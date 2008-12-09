////////////////////////////////////////////////////////////////////////////
//  
//  KRepton - the classic Repton game for KDE
//  
//  Copyright (c) 2008 Jonathan Marten <jjm@keelhaul.demon.co.uk>
//  
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  It is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING for further details.
//  If not, write to the Free Software Foundation, Inc., 59 Temple Place -
//  Suite 330, Boston, MA 02111-1307, USA.
//
////////////////////////////////////////////////////////////////////////////

#include <kdebug.h>

#include <qcolor.h>
#include <qfile.h>
#include <qcstring.h>

#include "map2.h"
#include "sprites.h"

#include "importerbbc.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  File and map parameters						//
//									//
//////////////////////////////////////////////////////////////////////////

#define BBC_NUM_MAPS		8
#define BBC_MAP_WIDTH		28
#define BBC_MAP_HEIGHT		24
#define BBC_FILE_SIZE		0x2620
#define BBC_PW_MAXLEN		7
#define BBC_MAX_TELEPORTS	4
#define BBC_NUM_COLOURS		4
#define BBC_NUM_SPRITES		Obj::num_sprites

#define field(v,s,l)		((v>>s) & ((1<<l)-1))	// Extract bit field


static QColor BBCcolours[] =
{
    Qt::black,						// 0
    Qt::red,						// 1
    Qt::green,						// 2
    Qt::yellow,						// 3
    Qt::blue,						// 4
    Qt::magenta,					// 5
    Qt::cyan,						// 6
    Qt::white						// 7
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Format information							//
//									//
//////////////////////////////////////////////////////////////////////////

class BBCFormatInfo
{
public:
    static const ImportManager::formatInfo myinfo;

protected:
    static ImporterBase *create();

private:
    static BBCFormatInfo initobj;
    BBCFormatInfo();
};


const ImportManager::formatInfo BBCFormatInfo::myinfo =
{
    "bbc",						// key
    "BBC Repton 3",					// name
    QString::null,					// url
    "BBC Repton 3 (and sequels) data files always contain eight episodes, each 28x24 in size. \
The graphics are in four colours.\
<p>\
The import requires a single data file, which should be 9760 bytes in size.",
							// notes
    BBC_FILE_SIZE,					// filesize
    &BBCFormatInfo::create				// createfunc
};


BBCFormatInfo BBCFormatInfo::initobj;


BBCFormatInfo::BBCFormatInfo()
{
    ImportManager::self()->add(&BBCFormatInfo::myinfo);
}


ImporterBase *BBCFormatInfo::create()
{
    ImporterBase *ib = new ImporterBBC();
    return (ib);
}


const ImportManager::formatInfo *ImporterBBC::formatInfo()
{
    return (&BBCFormatInfo::myinfo);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor						//
//									//
//////////////////////////////////////////////////////////////////////////

ImporterBBC::ImporterBBC()
    : ImporterBase()
{
}


ImporterBBC::~ImporterBBC()
{
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Perform the import							//
//									//
//////////////////////////////////////////////////////////////////////////

bool ImporterBBC::doImport(QFile &f,Episode *episode,Sprites *sprites,MapList *maplist,QString *results)
{
    kdDebug(0) << k_funcinfo << endl;

    QByteArray data = f.readAll();			// grab the complete file
    int ptr;						// data pointer into that

    for (int i = 0; i<BBC_NUM_MAPS; ++i)
    {
        ptr = 0x0000 + i*8;				// point to password
        QCString pw;
        int l = 0;
        while (l<=BBC_PW_MAXLEN)
        {
            unsigned char b = data[ptr] ^ (63-ptr);
            if (b=='\r') break;
            pw += b;
            ++ptr;
            ++l;
        }
        if (pw.length()<1) pw.sprintf("%c",i+'A');	// substitute blank passwords

        MapEdit *m = new MapEdit(BBC_MAP_WIDTH,BBC_MAP_HEIGHT,pw);

        ptr = 0x0040 + i*2;				// point to time limit
        int t = ((int) data[ptr]) + ((int) data[ptr+1])*256;
        m->changeTime(t);

        ptr = 0x0050 + i*2;				// point to edit code
        // not used in this application

        ptr = 0x0060 + i*16;				// point to teleport data
        for (int t = 0; t<BBC_MAX_TELEPORTS; ++t)
        {
            unsigned int sx = data[ptr+(t*4)+0];
            if (sx==0xFF) continue;			// no teleport here
            unsigned int sy = data[ptr+(t*4)+1];
            unsigned int dx = data[ptr+(t*4)+2];
            unsigned int dy = data[ptr+(t*4)+3];

            m->transporterInsert(sx+1,sy+1,dx+1,dy+1);
        }

        ptr = 0x0100 + i*420;				// point to map data
        for (int l = 0; l<(BBC_MAP_WIDTH*BBC_MAP_HEIGHT); l += 8)
        {
            unsigned char raw[5];			// read 5 raw bytes
            for (int j = 0; j<5; ++j) raw[j] = data[ptr++];

            unsigned char cell[8];			// unpack into 8 cells
            cell[0] = field(raw[0],0,5);
            cell[1] = field(raw[1],0,2)<<3 | field(raw[0],5,3);
            cell[2] = field(raw[1],2,5);
            cell[3] = field(raw[2],0,4)<<1 | field(raw[1],7,1);
            cell[4] = field(raw[3],0,1)<<4 | field(raw[2],4,4);
            cell[5] = field(raw[3],1,5);
            cell[6] = field(raw[4],0,3)<<2 | field(raw[3],6,2);
            cell[7] = field(raw[4],3,5);

            for (int j = 0; j<=7; ++j)			// output the cells
            {
                int cx = (l+j) % BBC_MAP_WIDTH;
                int cy = (l+j) / BBC_MAP_WIDTH;
                m->setCell(cx,cy,static_cast<Obj::Type>(cell[j]));
            }
        }

        maplist->append(m);
    }

    //  TODO: currently KRepton has one sprite bitmap and therefore one colour
    //  palette for all 8 screens, whereas the BBC R3 file format allows for 
    //  different palette for each screen.  This is not so important for the
    //  R3 sequels, where each episode had a single colour scheme, but for
    //  original R3 and also for R1 (if I ever get around to importing that),
    //  the colors are rather monotonous.

    QColor palette[BBC_NUM_COLOURS];
    ptr = 0x00E0;					// point to colour palette
    for (int c = 0; c<BBC_NUM_COLOURS; ++c)
    {
        palette[c] = BBCcolours[(int) data[ptr+c]];
    }

    ptr = 0x0E20;					// point to sprite data
    for (int c = 0; c<BBC_NUM_SPRITES; ++c)
    {
        Obj::Type obj = static_cast<Obj::Type>(c);

        for (int j = 0; j<=127; ++j)
        {
            int sx = 2*((j%32)/8)*4;
            int sy = ((j%8)+8*(j/32));
            unsigned char temp = data[ptr++];

            for (int k = 0; k<=3; ++k)
            {
                int col = (((temp&0x80)>>3)+(temp&0x08))>>3;
                sprites->setPixel(obj,sx,sy,palette[col]);
                ++sx;
                sprites->setPixel(obj,sx,sy,palette[col]);
                ++sx;
                temp <<= 1;
            }
        }
    }

    return (true);
}
