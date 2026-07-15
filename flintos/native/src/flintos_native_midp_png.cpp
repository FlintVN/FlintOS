
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "flint_array_object.h"
#include "flintos_native_midp.h"

extern "C" size_t tinfl_decompress_mem_to_mem(
    void *output,
    size_t outputLength,
    const void *source,
    size_t sourceLength,
    int flags
);

static constexpr int TINFL_PARSE_ZLIB_HEADER = 1;
static constexpr int TINFL_NON_WRAPPING_OUTPUT = 4;
static constexpr size_t TINFL_FAILED = (size_t)-1;

static uint32_t ReadBigEndian32(const uint8_t *data) {
    return (uint32_t)data[0] << 24
        | (uint32_t)data[1] << 16
        | (uint32_t)data[2] << 8
        | data[3];
}

static int PaethPredictor(int left, int above, int upperLeft) {
    int prediction = left + above - upperLeft;
    int leftDistance = abs(prediction - left);
    int aboveDistance = abs(prediction - above);
    int upperLeftDistance = abs(prediction - upperLeft);
    if(leftDistance <= aboveDistance && leftDistance <= upperLeftDistance)
        return left;
    if(aboveDistance <= upperLeftDistance)
        return above;
    return upperLeft;
}

static void WriteRgb565(uint8_t *output, int red, int green, int blue) {
    int pixel = ((blue >> 3) << 11) | ((green >> 2) << 5) | (red >> 3);
    output[0] = pixel >> 8;
    output[1] = pixel;
}

static bool AppendChunk(
        uint8_t **buffer,
        size_t *length,
        size_t *capacity,
        const uint8_t *data,
        size_t dataLength) {
    if(dataLength > SIZE_MAX - *length)
        return false;
    size_t required = *length + dataLength;
    if(required > *capacity) {
        size_t newCapacity = required + required / 2 + 1024;
        if(newCapacity < required)
            return false;
        uint8_t *resized = (uint8_t *)realloc(*buffer, newCapacity);
        if(resized == NULL)
            return false;
        *buffer = resized;
        *capacity = newCapacity;
    }
    memcpy(*buffer + *length, data, dataLength);
    *length = required;
    return true;
}

jbyteArray NativeMidpPng_Decode(
        FNIEnv *env,
        jbyteArray data,
        jint offset,
        jint length,
        jintArray dimensions) {
    if(data == NULL || dimensions == NULL)
        return NULL;
    if(dimensions->getLength() < 3)
        return NULL;
    if(offset < 0 || length < 8 || (uint32_t)offset > data->getLength()
            || (uint32_t)length > data->getLength() - (uint32_t)offset)
        return NULL;

    const uint8_t *png = (const uint8_t *)data->getData() + offset;
    static const uint8_t signature[] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    if(memcmp(png, signature, sizeof(signature)) != 0)
        return NULL;

    int width = 0;
    int height = 0;
    int bitDepth = 0;
    int colorType = 0;
    int interlace = 0;
    uint8_t palette[256 * 3] = {};
    uint8_t transparency[256] = {};
    int paletteEntries = 0;
    int transparencyEntries = 0;
    uint8_t *compressed = NULL;
    size_t compressedLength = 0;
    size_t compressedCapacity = 0;

    uint32_t position = 8;
    while(position + 12 <= (uint32_t)length) {
        uint32_t chunkLength = ReadBigEndian32(png + position);
        if(chunkLength > (uint32_t)length - position - 12)
            break;
        const uint8_t *chunkType = png + position + 4;
        const uint8_t *chunkData = png + position + 8;
        if(memcmp(chunkType, "IHDR", 4) == 0 && chunkLength == 13) {
            width = ReadBigEndian32(chunkData);
            height = ReadBigEndian32(chunkData + 4);
            bitDepth = chunkData[8];
            colorType = chunkData[9];
            interlace = chunkData[12];
        }
        else if(memcmp(chunkType, "PLTE", 4) == 0) {
            paletteEntries = chunkLength / 3;
            if(paletteEntries > 256)
                paletteEntries = 256;
            memcpy(palette, chunkData, paletteEntries * 3);
        }
        else if(memcmp(chunkType, "tRNS", 4) == 0) {
            transparencyEntries = chunkLength;
            if(transparencyEntries > 256)
                transparencyEntries = 256;
            memcpy(transparency, chunkData, transparencyEntries);
        }
        else if(memcmp(chunkType, "IDAT", 4) == 0) {
            if(!AppendChunk(
                    &compressed,
                    &compressedLength,
                    &compressedCapacity,
                    chunkData,
                    chunkLength)) {
                free(compressed);
                return NULL;
            }
        }
        else if(memcmp(chunkType, "IEND", 4) == 0)
            break;
        position += chunkLength + 12;
    }

    if(width <= 0 || height <= 0 || width > 2048 || height > 2048
            || bitDepth != 8 || interlace != 0 || compressedLength == 0) {
        free(compressed);
        return NULL;
    }

    int channels;
    switch(colorType) {
        case 0: channels = 1; break;
        case 2: channels = 3; break;
        case 3: channels = 1; break;
        case 4: channels = 2; break;
        case 6: channels = 4; break;
        default:
            free(compressed);
            return NULL;
    }

    size_t rowLength = (size_t)width * channels;
    if(rowLength > SIZE_MAX / height || rowLength + 1 > SIZE_MAX / height) {
        free(compressed);
        return NULL;
    }
    size_t inflatedLength = (rowLength + 1) * height;
    uint8_t *inflated = (uint8_t *)malloc(inflatedLength);
    if(inflated == NULL) {
        free(compressed);
        return NULL;
    }
    size_t decodedLength = tinfl_decompress_mem_to_mem(
        inflated,
        inflatedLength,
        compressed,
        compressedLength,
        TINFL_PARSE_ZLIB_HEADER | TINFL_NON_WRAPPING_OUTPUT
    );
    free(compressed);
    if(decodedLength == TINFL_FAILED || decodedLength != inflatedLength) {
        free(inflated);
        return NULL;
    }

    uint8_t *pixels = (uint8_t *)malloc(rowLength * height);
    if(pixels == NULL) {
        free(inflated);
        return NULL;
    }
    uint8_t *previous = NULL;
    for(int y = 0; y < height; y++) {
        const uint8_t *source = inflated + (rowLength + 1) * y;
        int filter = *source++;
        uint8_t *current = pixels + rowLength * y;
        if(filter > 4) {
            free(inflated);
            free(pixels);
            return NULL;
        }
        for(size_t x = 0; x < rowLength; x++) {
            int left = x >= (size_t)channels ? current[x - channels] : 0;
            int above = previous == NULL ? 0 : previous[x];
            int upperLeft = previous != NULL && x >= (size_t)channels
                ? previous[x - channels] : 0;
            int value = source[x];
            if(filter == 1)
                value += left;
            else if(filter == 2)
                value += above;
            else if(filter == 3)
                value += (left + above) / 2;
            else if(filter == 4)
                value += PaethPredictor(left, above, upperLeft);
            current[x] = value;
        }
        previous = current;
    }
    free(inflated);

    bool hasAlpha = colorType == 4 || colorType == 6
        || (colorType == 3 && transparencyEntries > 0);
    size_t pixelCount = (size_t)width * height;
    size_t rgbLength = pixelCount * 2;
    size_t alphaLength = hasAlpha ? (pixelCount + 1) / 2 : 0;
    if(rgbLength + alphaLength > UINT32_MAX) {
        free(pixels);
        return NULL;
    }
    jbyteArray output = env->newByteArray(rgbLength + alphaLength);
    if(output == NULL) {
        free(pixels);
        return NULL;
    }
    uint8_t *rgb = (uint8_t *)output->getData();
    uint8_t *alpha = rgb + rgbLength;
    if(hasAlpha)
        memset(alpha, 0, alphaLength);

    for(size_t index = 0; index < pixelCount; index++) {
        const uint8_t *source = pixels + index * channels;
        int red;
        int green;
        int blue;
        int opacity = 255;
        if(colorType == 0) {
            red = green = blue = source[0];
        }
        else if(colorType == 2) {
            red = source[0]; green = source[1]; blue = source[2];
        }
        else if(colorType == 3) {
            int paletteIndex = source[0];
            if(paletteIndex >= paletteEntries) {
                red = green = blue = 0;
            }
            else {
                red = palette[paletteIndex * 3];
                green = palette[paletteIndex * 3 + 1];
                blue = palette[paletteIndex * 3 + 2];
            }
            if(paletteIndex < transparencyEntries)
                opacity = transparency[paletteIndex];
        }
        else if(colorType == 4) {
            red = green = blue = source[0]; opacity = source[1];
        }
        else {
            red = source[0]; green = source[1]; blue = source[2]; opacity = source[3];
        }
        WriteRgb565(rgb + index * 2, red, green, blue);
        if(hasAlpha) {
            int value = opacity >> 4;
            if((index & 1) == 0)
                alpha[index / 2] = value;
            else
                alpha[index / 2] |= value << 4;
        }
    }
    free(pixels);

    int32_t *resultDimensions = dimensions->getData();
    resultDimensions[0] = width;
    resultDimensions[1] = height;
    resultDimensions[2] = hasAlpha ? 3 : 2;
    return output;
}
