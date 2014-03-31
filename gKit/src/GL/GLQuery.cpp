#include "GL/GLQuery.h"
namespace gk
{
std::string OverlapGPUSummary( const std::string &message, const GLCounterState& a, const GLCounterState& b )
{
    char tmp[1024];
    if(a.gpu_start < b.gpu_stop && a.gpu_stop > b.gpu_start)
        sprintf(tmp, "%s:    overlap [%6lu -- %6lu]", message.c_str(), std::max(a.gpu_start, b.gpu_start), std::min(a.gpu_stop, b.gpu_stop));
    else
        sprintf(tmp, "%s: no overlap [%6lu -- %6lu]", message.c_str(), std::max(a.gpu_start, b.gpu_start), std::min(a.gpu_stop, b.gpu_stop));
    
    return std::string(tmp);
}

std::string OverlapCPUSummary( const std::string &message, const GLCounterState& a, const GLCounterState& b )
{
    char tmp[1024];
    if(a.cpu_start < b.cpu_stop && a.cpu_stop > b.cpu_start)
        sprintf(tmp, "%s:    overlap [%6lu -- %6lu]", message.c_str(), std::max(a.cpu_start, b.cpu_start), std::min(a.cpu_stop, b.cpu_stop));
    else
        sprintf(tmp, "%s: no overlap [%6lu -- %6lu]", message.c_str(), std::max(a.cpu_start, b.cpu_start), std::min(a.cpu_stop, b.cpu_stop));
    
    return std::string(tmp);
}
};