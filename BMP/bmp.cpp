
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

void main()
{
    wstring sFileName;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    int Width, Height;
    RGBQUAD Palette[256];
    BYTE* inBuf;
    WORD* outBuf;
    HANDLE hInputFile, hOutFile;
    DWORD RW;

    cout << "Enter the full name, please: ";
    wcin >> sFileName;

    hInputFile = CreateFile(sFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hInputFile == INVALID_HANDLE_VALUE)
        return;

    hOutFile = CreateFile(L"Result.bmp", GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
    if (hOutFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hInputFile);
        return;
    }

    // Считываем инфу
    ReadFile(hInputFile, &bmpFileHeader, sizeof(bmpFileHeader), &RW, NULL);
    ReadFile(hInputFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &RW, NULL);
    ReadFile(hInputFile, Palette, 256 * sizeof(RGBQUAD), &RW, NULL);

    // Установим указатель на начало растра
    SetFilePointer(hInputFile, bmpFileHeader.bfOffBits, NULL, FILE_BEGIN);
    Width = bmpInfoHeader.biWidth;
    Height = bmpInfoHeader.biHeight;

    // Выделим память
    inBuf = new BYTE[Width];
    outBuf = new WORD[Width];

    // Заполним заголовки
    bmpFileHeader.bfOffBits = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader);
    bmpInfoHeader.biClrUsed = 32768;
    bmpInfoHeader.biBitCount = 16;
    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + 2 * Width * Height + Height * (2 * Width % 4);
   
    // Запишем заголовки
    WriteFile(hOutFile, &bmpFileHeader, sizeof(bmpFileHeader), &RW, NULL);
    WriteFile(hOutFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &RW, NULL);

    // Начнем преобразовывать
    for (int i = 0; i < Height; i++)
    {
        ReadFile(hInputFile, inBuf, sizeof(BYTE) * Width, &RW, NULL);
        for (int j = 0; j < Width; j++) {
            outBuf[j] = ((31 * Palette[inBuf[j]].rgbRed / 255) << 10) 
                + ((31 * Palette[inBuf[j]].rgbGreen / 255) << 5) + (31 * Palette[inBuf[j]].rgbBlue / 255);
        }

        WriteFile(hOutFile, outBuf, sizeof(WORD) * Width, &RW, NULL);

        // Пишем мусор для выравнивания
        WriteFile(hOutFile, Palette, (2 * Width) % 4, &RW, NULL);
        SetFilePointer(hInputFile, (2 * Width) % 4, NULL, FILE_CURRENT);
    }

    delete[] inBuf;
    delete[] outBuf;
    CloseHandle(hInputFile);
    CloseHandle(hOutFile);

    cout << "Updating has come to the end successfully!";
    system("pause");
}