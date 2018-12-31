#include <ArduinoSTL.h>

using namespace std;

void inv_kin(vector<float>& points);
void ppos2mrot(vector<float>& ppos);
vector<long> mrot2gtick(vector<float> mrot);
vector<long> gtick2ltick(vector<long> gtick);
