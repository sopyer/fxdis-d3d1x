/**************************************************************************
 *
 * Copyright 2010 Luca Barbieri
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "dxbc.h"
#include "sm4.h"
#include <iostream>
#include <fstream>

void usage()
{
    std::cerr << "Gallium Direct3D10/11 Shader Disassembler\n";
    std::cerr << "This program is free software, released under a MIT-like license\n";
    std::cerr << "Not affiliated with or endorsed by Microsoft in any way\n";
    std::cerr << "Latest version available from http://cgit.freedesktop.org/mesa/mesa/\n";
    std::cerr << "\n";
    std::cerr << "Usage: fxdis FILE\n";
    std::cerr << std::endl;
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        usage();
        return EXIT_FAILURE;
    }

    std::vector<char> data;
    FILE *pFile = NULL;
#ifdef _MSC_VER
    fopen_s(&pFile, argv[1], "rb" );
#else
    pFile = fopen(argv[1], "rb" );
#endif
    if ( !pFile )
    {
       printf("Could not open file: %s\n", pFile );
       return EXIT_FAILURE;
    }

    fseek(pFile, 0, SEEK_END);
    uint32_t nFileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    if (nFileSize < sizeof(dxbc_container_header))
    {
      printf("File is too small!\n");
      return EXIT_FAILURE;
    }

    data.resize(nFileSize);
    if (fread(&data[0], 1, nFileSize, pFile) != nFileSize)
    {
       printf("Failed reading file!\n");
       return EXIT_FAILURE;
    }
    fclose(pFile);

    dxbc_container* dxbc = dxbc_parse(&data[0], data.size());
    if(dxbc)
    {
        std::cout << *dxbc;
        dxbc_chunk_header* sm4_chunk = dxbc_find_shader_bytecode(&data[0], data.size());
        if(sm4_chunk)
        {
            sm4_program* sm4 = sm4_parse(sm4_chunk + 1, bswap_le32(sm4_chunk->size));
            if(sm4)
            {
                std::cout << *sm4;
                delete sm4;
            }
        }
        delete dxbc;
    }

    return EXIT_SUCCESS;
}
