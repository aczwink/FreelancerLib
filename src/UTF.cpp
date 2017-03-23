/*
 * Copyright (c) 2017 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of FreelancerLib.
 *
 * FreelancerLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FreelancerLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreelancerLib.  If not, see <http://www.gnu.org/licenses/>.
 */
//Class header
#include <ctime>
#include "../include/UTF.hpp"
//Namespaces
using namespace ACStdLib;
using namespace Freelancer;

struct UTFHeader
{
    uint32 signature; //"UTF "
    uint32 version; //= 0x101
    uint32 treeSegmentOffset;
    uint32 treeSegmentSize;
    uint32 unknown1; //= 0
    uint32 nodeHeaderSize; //= 44
    uint32 stringSegmentOffset;
    uint32 stringSegmentSize;
    //uint32 stringSegmentSizeUnpadded //excluding trailing zeros...
    uint32 dataSegmentOffset;
    uint32 unknown2; //= 0
    uint32 unknown3; //= 0
    //uint32 timestamp; //unix timestamp
    uint32 unknown5;
};

struct UTFNode
{
    uint32 unknown0;
    uint32 nameEntryOffset;
    uint32 flags;
    uint32 unknown1; //= 0
    uint32 offset;
    //uint32 paddedSize; //includes padding bytes
    uint32 size;
    uint32 unknownSize2;
    uint32 unknown4;
    //uint32 timestamp[3]; //unix timestamp
};

//Public methods
void UTF::LoadNodes(ASeekableInputStream &refInput)
{
    byte *pTreeSegment, *pStringTable;
    uint32 i, nNodes, flags;
    UTFHeader utfHeader;
    UTFNode utfNode;
    C8BitString nodeName;

    utfHeader.signature = refInput.ReadUInt32BE();
    utfHeader.version = refInput.ReadUInt32LE();
    utfHeader.treeSegmentOffset = refInput.ReadUInt32LE();
    utfHeader.treeSegmentSize = refInput.ReadUInt32LE();
    utfHeader.unknown1 = refInput.ReadUInt32LE();
    utfHeader.nodeHeaderSize = refInput.ReadUInt32LE();
    utfHeader.stringSegmentOffset = refInput.ReadUInt32LE();
    utfHeader.stringSegmentSize = refInput.ReadUInt32LE();
    refInput.Skip(4); //unpadded size of string segment i.e. size of string segment with trailing zeros removed
    utfHeader.dataSegmentOffset = refInput.ReadUInt32LE();
    utfHeader.unknown2 = refInput.ReadUInt32LE();
    utfHeader.unknown3 = refInput.ReadUInt32LE();
    refInput.Skip(4);
    utfHeader.unknown5 = refInput.ReadUInt32LE();

    ASSERT(utfHeader.signature == MAKE32_FROM4('U', 'T', 'F', ' '));
    ASSERT(utfHeader.version == 0x101);
    ASSERT(utfHeader.unknown1 == 0);
    ASSERT(utfHeader.nodeHeaderSize == 44);
    ASSERT(utfHeader.unknown2 == 0);
    ASSERT(utfHeader.unknown3 == 0);

    ASSERT(utfHeader.treeSegmentOffset == refInput.GetCurrentOffset()); //must be now at tree segment

    //read tree segment
    pTreeSegment = (byte *)MemAlloc(utfHeader.treeSegmentSize);
    refInput.ReadBytes(pTreeSegment, utfHeader.treeSegmentSize);

    CBufferInputStream treeInput(pTreeSegment, utfHeader.treeSegmentSize);

    ASSERT(utfHeader.stringSegmentOffset == refInput.GetCurrentOffset()); //must be now at string segment

    //read string table
    pStringTable = (byte *)MemAlloc(utfHeader.stringSegmentSize);
    refInput.ReadBytes(pStringTable, utfHeader.stringSegmentSize);

    CBufferInputStream stringInput(pStringTable, utfHeader.stringSegmentSize);
    CTextReader reader(stringInput);

    ASSERT(utfHeader.dataSegmentOffset == refInput.GetCurrentOffset()); //must be now at data segment

    nNodes = utfHeader.treeSegmentSize / utfHeader.nodeHeaderSize;
    for(i = 0; i < nNodes; i++)
    {
        utfNode.unknown0 = treeInput.ReadUInt32LE();
        utfNode.nameEntryOffset = treeInput.ReadUInt32LE();
        utfNode.flags = treeInput.ReadUInt32LE();
        utfNode.unknown1 = treeInput.ReadUInt32LE();
        utfNode.offset = treeInput.ReadUInt32LE();
        treeInput.Skip(4);
        utfNode.size = treeInput.ReadUInt32LE();
        utfNode.unknownSize2 = treeInput.ReadUInt32LE();
        treeInput.Skip(12);

        /*
        time_t tqwe = utfNode.unknown5;
        std::tm * ptm = std::localtime(&tqwe);
        char buffer[32];
// Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        stdOut << buffer << endl;
        exit(1);
        */

        ASSERT(utfNode.unknown1 == 0);
        ASSERT(utfNode.size == utfNode.unknownSize2);

        stringInput.SetCurrentOffset(utfNode.nameEntryOffset);
        nodeName = reader.ReadASCII_ZeroTerminated();

        stdOut << nodeName << endl;
    }

    MemFree(pTreeSegment);
    MemFree(pStringTable);
}