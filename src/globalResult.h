#ifndef __GLOBALRESULT_H__
#define __GLOBALRESULT_H__

#include <vector>
#include <string>

struct globalResultBlock{
    std::string name;
    std::string type;
    int llx, lly;
    int width, height;
};

struct globalResultConnection{
    std::vector<std::string> vertices;
    double weight;
};

class GlobalResult{
public:
    int blockCount;
    int connectionCount;
    int chipWidth, chipHeight;

    std::vector<globalResultBlock> blocks;
    std::vector<globalResultConnection> connections;

    GlobalResult();
    GlobalResult(std::string globalResultFile);
    GlobalResult(const GlobalResult &other);

    GlobalResult &operator = (const GlobalResult &other);
    bool operator == (const GlobalResult &other) const;
    friend std::ostream &operator << (std::ostream &os, const GlobalResult &gr);

    void readGlobalResult(std::istream &is);
    void readGlobalResult(std::string globalResultFile);
    void writeGlobalResult(std::ostream &os);
    void writeGlobalResult(std::string globalResultFile);
};

std::ostream &operator << (std::ostream &os, const GlobalResult &gr);

#endif // __GLOBALRESULT_H__