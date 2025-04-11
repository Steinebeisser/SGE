//
// Created by Geisthardt on 07.03.2025.
//

#include "utils/sge_math.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core/memory_control.h"
#include "core/sge_internal_logging.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int amount_chars_in_float(float num);


////////////////////
// VEC3 Functions //
////////////////////

vec3 sge_vec3_add(const vec3 a, const vec3 b) {
        const vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
        return result;
}

vec3 sge_vec3_sub(const vec3 a, const vec3 b) {
        const vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
        return result;
}

vec3 sge_vec3_normalize(vec3 vector) {
        float magnitude = sqrt(squared(vector.x) + squared(vector.y) + squared(vector.z));

        if (magnitude == 0) {
                return (vec3){0.0, 0.0, 0.0};
        }

        return (vec3){
                vector.x / magnitude,
                vector.y / magnitude,
                vector.z / magnitude
        };
}

vec3 sge_vec3_cross(vec3 vector1, vec3 vector2) {
        return (vec3){
                vector1.y * vector2.z - vector1.z * vector2.y,
                vector1.z * vector2.x - vector1.x * vector2.z,
                vector1.x * vector2.y - vector1.y * vector2.x
            };
}

vec3 sge_vec3_scale(vec3 vector1, float scale_factor) {
        vec3 scaled = {
                vector1.x * scale_factor,
                vector1.y * scale_factor,
                vector1.z * scale_factor,
        };


        return scaled;
}

vec3 sge_vec3_rotate_yaw_pitch(vec3 vector, vec3 rotation) {
        vec3 rotated = sge_vec3_rotate_yaw(vector, rotation);
        rotated = sge_vec3_rotate_pitch(rotated, rotation);
        return rotated;
}

vec3 sge_vec3_rotate_pitch(vec3 vector, vec3 rotation) {
        float radiant = -rotation.x * M_PI / 180.0f;
        float c = cos(radiant);
        float s = sin(radiant);

        vec3 rotated = {
                vector.x,
                vector.y * c - vector.z * s,
                vector.y * s + vector.z * c
        };

        return rotated;
}

vec3 sge_vec3_rotate_yaw(vec3 vector, vec3 rotation) {
        float radiant = -rotation.y * M_PI / 180.0f;
        float c = cos(radiant);
        float s = sin(radiant);

        vec3 rotated = {
                vector.x * c + vector.z * s,
                vector.y,
                -vector.x * s + vector.z * c
        };

        return rotated;
}


//////////////////
// M4 functions //
//////////////////

void sge_m4_set_identity(m4 matrix) {
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        if (j == i) {
                                matrix[i][j] = 1;
                        } else {
                                matrix[i][j] = 0;
                        }
                }
        }
}

void sge_m4_set_value(m4 matrix, int row, int col, float value) {
        if (row > 3 || col > 3 || row < 0 || col < 0) {
                log_internal_event(LOG_LEVEL_ERROR, "invalid dimensions in m4");
                return;
        }
        matrix[row][col] = value;
}

float sge_m4_get_value(m4 matrix, int row, int col) {
        if (row > 3 || col > 3 || row < 0 || col < 0) {
                log_internal_event(LOG_LEVEL_ERROR, "invalid dimensions in m4");
                return 0;
        }
        float value = matrix[row][col];
        return value;
}

void sge_m4_set_translate(m4 matrix, vec3 translate) {
        matrix[0][3] = translate.x;
        matrix[1][3] = translate.y;
        matrix[2][3] = translate.z;
}

void sge_m4_set_scale(m4 matrix, vec3 scale) {
        matrix[0][0] = scale.x;
        matrix[1][1] = scale.y;
        matrix[2][2] = scale.z;
}

void sge_m4_set_rotate_x(m4 matrix, float angle) {
        float c = cos(angle * M_PI / 180.f);
        float s = sin(angle * M_PI / 180.0f);
        matrix[1][1] = c;
        matrix[1][2] = -s;
        matrix[2][1] = s;
        matrix[2][2] = c;
}
void sge_m4_set_rotate_y(m4 matrix, float angle) {
        float c = cos(angle * M_PI / 180.f);
        float s = sin(angle * M_PI / 180.0f);
        matrix[0][0] = c;
        matrix[0][2] = s;
        matrix[2][0] = -s;
        matrix[2][2] = c;
}

void sge_m4_set_rotate_z(m4 matrix, float angle) {
        float c = cos(angle * M_PI / 180.f);
        float s = sin(angle * M_PI / 180.0f);
        matrix[0][0] = c;
        matrix[1][0] = s;
        matrix[0][1] = -s;
        matrix[1][1] = c;
}
void sge_m4_multiply(m4 m_result, m4 m_a, m4 m_b) {
        m4 temp;
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        temp[i][j] = 0;
                        for (int l = 0; l < 4; l++) {
                                temp[i][j] += m_a[i][l] * m_b[l][j];
                        }
                }
        }
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        m_result[i][j] = temp[i][j];
                }
        }
}

void sge_m4_copy(m4 m_to_copy, m4 m_from_copy) {
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        m_to_copy[i][j] = m_from_copy[i][j];
                }
        }

}

void sge_m4_transpose(m4 matrix) {
        for (int i = 0; i < 4; i++) {
                for (int j = i + 1; j < 4; j++) {
                        const float temp = matrix[i][j];
                        matrix[i][j] = matrix[j][i];
                        matrix[j][i] = temp;
                }
        }
}

void sge_m4_set_rotate(m4 matrix, vec3 rotation) {
        m4 rx, ry, rz, temp;

        sge_m4_set_identity(rx);
        sge_m4_set_identity(ry);
        sge_m4_set_identity(rz);
        sge_m4_set_identity(temp);

        sge_m4_set_rotate_x(rx, rotation.x);
        sge_m4_set_rotate_y(ry, rotation.y);
        sge_m4_set_rotate_z(rz, rotation.z);

        sge_m4_multiply(temp, rx, ry);
        sge_m4_multiply(matrix, temp, rz);
}

void sge_m4_print(m4 matrix) {
        int longest_num = 0;
        char matrix_string[1024];
        zero_memory(matrix_string, sizeof(matrix_string), 0);
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        int number_length = amount_chars_in_float(matrix[i][j]);
                        if (number_length > longest_num) {
                                longest_num = number_length;
                        }
                }
        }

        int box_width = (longest_num * 4) + (4 * 4);

        for (int i = 0; i < box_width; ++i) {
                snprintf(matrix_string + strlen(matrix_string), sizeof(matrix_string), "-");
        }
        snprintf(matrix_string + strlen(matrix_string), sizeof(matrix_string), "\n");
        for (int i = 0; i < 4; ++i) {
                snprintf(matrix_string + strlen(matrix_string), sizeof(matrix_string), "| %*.4f | %*.4f | %*.4f | %*.4f |\n",
                        longest_num, matrix[i][0],
                        longest_num, matrix[i][1],
                        longest_num, matrix[i][2],
                        longest_num, matrix[i][3]);
                for (int j = 0; j < box_width; ++j) {
                        snprintf(matrix_string + strlen(matrix_string), sizeof(matrix_string), "-");
                }
                snprintf(matrix_string + strlen(matrix_string), sizeof(matrix_string), "\n");
        }
        snprintf(matrix_string + strlen(matrix_string), sizeof(matrix_string), "\n");
                        //printf("%f ", matrix[i][j]);

        printf("%s", matrix_string);
}

vec4 sge_m4_transform_vec4(m4 matrix, vec4 vec) {
        vec4 result;
        result.x = matrix[0][0] * vec.x + matrix[0][1] * vec.y + matrix[0][2] * vec.z + matrix[0][3] * vec.a;
        result.y = matrix[1][0] * vec.x + matrix[1][1] * vec.y + matrix[1][2] * vec.z + matrix[1][3] * vec.a;
        result.z = matrix[2][0] * vec.x + matrix[2][1] * vec.y + matrix[2][2] * vec.z + matrix[2][3] * vec.a;
        result.a = matrix[3][0] * vec.x + matrix[3][1] * vec.y + matrix[3][2] * vec.z + matrix[3][3] * vec.a;
        return result;
}

void sge_vec4_print(vec4 vector) {
        printf("-------\n");
        printf("| %4.2f |\n", vector.x);
        printf("| %4.2f |\n", vector.y);
        printf("| %4.2f |\n", vector.z);
        printf("| %4.2f |\n", vector.a);
        printf("-------\n");
}

void sge_vec3_print(vec3 vector) {
        printf("-------\n");
        printf("| %4.2f |\n", vector.x);
        printf("| %4.2f |\n", vector.y);
        printf("| %4.2f |\n", vector.z);
        printf("-------\n");
}

int amount_chars_in_float(float num) {
       //todo
        //printf("%f\n", num);
        if (num < 0 || num == -0) {
                return 6;
        }
        return 6;
}


size_t squared(size_t num) {
        return num * num;
}


/*
Calculated using

void generate_crc32_table() {
        uint32_t polynomial = 0xEDB88320;
        for (uint32_t i = 0; i < 256; i++) {
                uint32_t crc = i;
                for (int j = 0; j < 8; j++) {
                        crc = (crc & 1) ? (crc >> 1) ^ polynomial : crc >> 1;
                }
                crc32_table[i] = crc;

                printf("0x%08X, ", crc);

                if ((i + 1) % 4 == 0) {
                        printf("\n");
                }
        }
        printf("\n");
}
*/
unsigned int crc32_table[] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
        0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
        0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
        0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
        0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
        0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
        0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
        0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
        0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
        0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
        0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
        0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
        0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
        0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
        0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
        0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
        0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
        0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
        0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
        0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
        0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
        0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};
