#ifndef ENGINE_PIPELINE_H
#define ENGINE_PIPELINE_H

#include <string>
#include <vector>

class Pipeline {
public:
    Pipeline(const std::string& vertexShader, const std::string& fragmentShader);
    ~Pipeline();

private:
    static std::vector<char> readFile(const std::string& filename);
    void createGraphicsPipeline(const std::string &vertexShader, const std::string &fragmentShader);

};

#endif //ENGINE_PIPELINE_H
