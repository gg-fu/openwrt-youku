/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdbool.h>
#include "string_utils.h"
#include "HAL_Linux.h"
#include "log.h"
#define LITE_isdigit(c)             (((c) <= '9' && (c) >= '0') ? (true) : (false))

void LITE_hexbuf_convert(unsigned char *digest, char *out, int in_len, int uppercase)
{
    static char    *zEncode[] = {"0123456789abcdef", "0123456789ABCDEF"};
    int             j = 0;
    int             i = 0;
    int             idx = uppercase ? 1 : 0;
    for (i = 0; i < in_len; i ++) {
        int         a = digest[i];
        out[j++] = zEncode[idx][(a >> 4) & 0xf];
        out[j++] = zEncode[idx][a & 0xf];
    }
}
static uint8_t _hexval_of_char(char hex)
{
    if (LITE_isdigit(hex)) {
        return (hex - '0');
    }
    if (hex >= 'a' && hex <= 'f') {
        return (hex - 'a' + 10);
    }
    if (hex >= 'A' && hex <= 'F') {
        return (hex - 'A' + 10);
    }
    return 0;
}
void LITE_hexstr_convert(char *input, int input_len, unsigned char *output, int output_len)
{
    int             i = 0;
    uint8_t         ch0, ch1;
    if (input_len % 2 != 0) {
        UTILS_ERR("hexstr length (%d) is not even", input_len);
        return;
    }
    while (i < input_len / 2 && i < output_len) {
        ch0 = _hexval_of_char((char)input[2 * i]);
        ch1 = _hexval_of_char((char)input[2 * i + 1]);
        output[i] = (ch0 << 4 | ch1);
        i++;
    }
}
void LITE_replace_substr(char originalString[], char key[], char swap[])
{
    int         lengthOfOriginalString, lengthOfKey, lengthOfSwap, i, j, flag;
    char        tmp[512];
    lengthOfOriginalString = strlen(originalString);
    lengthOfKey = strlen(key);
    lengthOfSwap = strlen(swap);
    if (lengthOfOriginalString >= 512 || (lengthOfOriginalString-lengthOfKey+lengthOfSwap) >= 512) {
        return;
    }
    for (i = 0; i <= lengthOfOriginalString - lengthOfKey; i++) {
        flag = 1;
        for (j  = 0; j < lengthOfKey; j++) {
            if (originalString[i + j] != key[j]) {
                flag = 0;
                break;
            }
        }
        if (flag) {
            strcpy(tmp, originalString);
            strcpy(&tmp[i], swap);
            if (strlen(&originalString[i  + lengthOfKey]) >= (512 - i - lengthOfSwap)) {
                return;
            }
            strcpy(&tmp[i + lengthOfSwap], &originalString[i  + lengthOfKey]);
            strcpy(originalString, tmp);
            i += lengthOfSwap - 1;
            lengthOfOriginalString = strlen(originalString);
        }
    }
}
int LITE_get_randstr(char *random, int length)
{
    int index = 0;
    if (random == NULL || length <= 0) {
        UTILS_ERR("Invalid Parameter");
        return -1;
    }
    HAL_Srandom(HAL_UptimeMs());
    for (index = 0; index < length; index++) {
        switch (HAL_Random(3)) {
            case 0: {
                random[index] = 'A' + HAL_Random(26);
            }
            break;
            case 1: {
                random[index]  = 'a' + HAL_Random(26);
            }
            break;
            case 2: {
                random[index] = '0' + HAL_Random(10);
            }
            break;
            default: {
                UTILS_ERR("Should Not Execute Here");
                return -1;
            }
        }
    }
    return 0;
}

