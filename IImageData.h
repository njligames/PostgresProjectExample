//
// Created by James Folk on 4/14/25.
//

#include <string>
#include <vector>

#ifndef MYPROJECT_IIMAGEDATA_H
#define MYPROJECT_IIMAGEDATA_H

namespace NJLIC {

    class IImageData {
    public:
        virtual ~IImageData() = default;

        // Getters
        virtual const std::string &getFilename() const = 0;
        virtual int getRows() const = 0;
        virtual int getCols() const = 0;
        virtual int getComps() const = 0;
        virtual const std::vector<unsigned char>& getData() const = 0;

        // Setters
        virtual void setFilename(const std::string& filename) = 0;
        virtual void setRows(int rows) = 0;
        virtual void setCols(int cols) = 0;
        virtual void setComps(int comps) = 0;
        virtual void setData(const std::vector<unsigned char>& data) = 0;
    };
}


#endif //MYPROJECT_IIMAGEDATA_H
