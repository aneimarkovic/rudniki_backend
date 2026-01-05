//
// Created by Anei Markovic on 1/4/26.
//
#ifndef RUDNIKI_BACKEND_SHA256_H
#define RUDNIKI_BACKEND_SHA256_H
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

class SHA256 {
private:
    uint32_t state[8];
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    const static uint32_t k[64];
    uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }
    uint32_t choose(uint32_t e, uint32_t f, uint32_t g) { return (e & f) ^ (~e & g); }
    uint32_t majority(uint32_t a, uint32_t b, uint32_t c) { return (a & (b | c)) | (b & c); }
    uint32_t sig0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
    uint32_t sig1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }
    void transform() {
        uint32_t m[64], a, b, c, d, e, f, g, h, t1, t2;
        for (int i = 0, j = 0; i < 16; ++i, j += 4)
            m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
        for (int i = 16; i < 64; ++i) m[i] = sig1(m[i - 2]) + m[i - 7] + sig0(m[i - 15]) + m[i - 16];
        a = state[0]; b = state[1]; c = state[2]; d = state[3]; e = state[4]; f = state[5]; g = state[6]; h = state[7];
        for (int i = 0; i < 64; ++i) {
            t1 = h + sig1(e) + choose(e, f, g) + k[i] + m[i];
            t2 = sig0(a) + majority(a, b, c);
            h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e; state[5] += f; state[6] += g; state[7] += h;
    }
public:
    SHA256() {
        datalen = 0; bitlen = 0;
        state[0] = 0x6a09e667; state[1] = 0xbb67ae85; state[2] = 0x3c6ef372; state[3] = 0xa54ff53a;
        state[4] = 0x510e527f; state[5] = 0x9b05688c; state[6] = 0x1f83d9ab; state[7] = 0x5be0cd19;
    }
    void update(const string& s) {
        for (size_t i = 0; i < s.length(); ++i) {
            data[datalen++] = s[i];
            if (datalen == 64) { transform(); bitlen += 512; datalen = 0; }
        }
    }
    string final() {
        uint32_t i = datalen;
        if (datalen < 56) { data[i++] = 0x80; while (i < 56) data[i++] = 0x00; }
        else { data[i++] = 0x80; while (i < 64) data[i++] = 0x00; transform(); memset(data, 0, 56); }
        bitlen += datalen * 8;
        data[63] = bitlen; data[62] = bitlen >> 8; data[61] = bitlen >> 16; data[60] = bitlen >> 24;
        data[59] = bitlen >> 32; data[58] = bitlen >> 40; data[57] = bitlen >> 48; data[56] = bitlen >> 56;
        transform();
        stringstream ss;
        for (i = 0; i < 8; ++i) ss << hex << setfill('0') << setw(8) << state[i];
        return ss.str();
    }
};
#endif //RUDNIKI_BACKEND_SHA256_H