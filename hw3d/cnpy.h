//Copyright (C) 2011  Carl Rogers
//Released under MIT License
//license available in LICENSE file, or at http://www.opensource.org/licenses/mit-license.php

#ifndef LIBCNPY_H_
#define LIBCNPY_H_

#include<string>
#include<stdexcept>
#include<vector>
#include<fstream>
#include<typeinfo>
#include<cassert>
#include<map>
#include<stdint.h>
#include<numeric>

namespace cnpy {

    struct NpyArray {
        NpyArray(const std::vector<size_t>& _shape, size_t _word_size, bool _fortran_order) :
            shape(_shape), word_size(_word_size), fortran_order(_fortran_order)
        {
            num_vals = 1;
            for(size_t i = 0;i < shape.size();i++) num_vals *= shape[i];
            data_holder = std::shared_ptr<std::vector<char>>(
                new std::vector<char>(num_vals * word_size));
        }

        NpyArray() : shape(0), word_size(0), fortran_order(0), num_vals(0) { }

        template<typename T>
        T* data() {
            return reinterpret_cast<T*>(&(*data_holder)[0]);
        }

        template<typename T>
        const T* data() const {
            return reinterpret_cast<T*>(&(*data_holder)[0]);
        }

        template<typename T>
        std::vector<T> as_vec() const {
            const T* p = data<T>();
            return std::vector<T>(p, p+num_vals);
        }

        size_t num_bytes() const {
            return data_holder->size();
        }

        std::shared_ptr<std::vector<char>> data_holder;
        std::vector<size_t> shape;
        size_t word_size;
        bool fortran_order;
        size_t num_vals;
    };
   
    using npz_t = std::map<std::string, NpyArray>; 

    char BigEndianTest();
    char map_type(const std::type_info& t);
    template<typename T> std::vector<char> create_npy_header(const std::vector<size_t>& shape);

    template<typename T> std::vector<char>& operator+=(std::vector<char>& lhs, const T rhs) {
        //write in little endian
        for(size_t byte = 0; byte < sizeof(T); byte++) {
            char val = *((char*)&rhs+byte); 
            lhs.push_back(val);
        }
        return lhs;
    }

    template<> std::vector<char>& operator+=(std::vector<char>& lhs, const std::string rhs);
    template<> std::vector<char>& operator+=(std::vector<char>& lhs, const char* rhs);


    template<typename T>
    void npy_save(std::string fname, const T* data, const std::vector<size_t> shape) {

        std::ofstream file( fname,std::ios_base::binary );

        const std::vector<char> header = create_npy_header<T>( shape );
        const size_t nels = std::accumulate(shape.begin(),shape.end(),(size_t)1,std::multiplies<size_t>());

        file.write( header.data(),header.size() );
        file.write( reinterpret_cast<const char*>(data),sizeof(T) * nels );
        file.close();
    }

    template<typename T> void npy_save(std::string fname, std::vector<T> data) {
        std::vector<size_t> shape;
        shape.push_back(data.size());
        npy_save(std::move(fname), &data[0], std::move(shape));
    }

    template<typename T> std::vector<char> create_npy_header(const std::vector<size_t>& shape) {  

        std::vector<char> dict;
        dict += "{'descr': '";
        dict += BigEndianTest();
        dict += map_type(typeid(T));
        dict += std::to_string(sizeof(T));
        dict += "', 'fortran_order': False, 'shape': (";
        dict += std::to_string(shape[0]);
        for(size_t i = 1;i < shape.size();i++) {
            dict += ", ";
            dict += std::to_string(shape[i]);
        }
        if(shape.size() == 1) dict += ",";
        dict += "), }";
        //pad with spaces so that preamble+dict is modulo 16 bytes. preamble is 10 bytes. dict needs to end with \n
        int remainder = 16 - (10 + dict.size()) % 16;
        dict.insert(dict.end(),remainder,' ');
        dict.back() = '\n';

        std::vector<char> header;
        header += (char) 0x93;
        header += "NUMPY";
        header += (char) 0x01; //major version of numpy format
        header += (char) 0x00; //minor version of numpy format
        header += (uint16_t) dict.size();
        header.insert(header.end(),dict.begin(),dict.end());

        return header;
    }

}

#endif
