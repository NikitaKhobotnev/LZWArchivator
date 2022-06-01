export module MethodsArchiver;

import <iostream>;
import <string>;
import <map>;
import <stdexcept>;
import <vector>;


export std::vector<uint32_t> compressLZW(const std::string uncompressed) {

    uint32_t dictSize = 256;
    std::map<std::string, uint32_t> dictionary;
    for (uint32_t i = 0; i < dictSize; i++)
        dictionary[std::string(1, i)] = i;
    std::vector<uint32_t> compressed = std::vector<uint32_t>(0, 0);

    std::string w = "";
    for (auto iter = uncompressed.begin();
        iter != uncompressed.end(); iter++)
    {
        char c = *iter;
        std::string wc = w + c;
        if (dictionary.count(wc))
            w = wc;
        else {
            compressed.emplace_back(dictionary[w]);
            dictionary[wc] = dictSize++;
            w = std::string(1, c);
        }
    }
    if (!w.empty())
        compressed.push_back(dictionary[w]);

    std::string final_compress = "";


    return compressed;
}

export std::string decompressLZW(std::vector<uint32_t> compressed) {
    uint32_t dictSize = 256;
    std::map<uint32_t, std::string> dictionary;
    for (uint32_t i = 0; i < dictSize; i++)
        dictionary[i] = std::string(1, i);

    std::string w{ (char)(compressed.at(0))}; //???
    std::string result = w;
    std::string entry;

    for (auto iter = compressed.begin() + 1;
        iter != compressed.end(); iter++)
    {
        int k = *iter;
        if (dictionary.count(k))
            entry = dictionary[k];
        else if (k == dictSize)
            entry = w + w[0];
        else throw std::runtime_error("Bad compressed"); 

        result += entry;

        dictionary[dictSize++] = w + entry[0];

        w = entry;
    }

    return result;
}