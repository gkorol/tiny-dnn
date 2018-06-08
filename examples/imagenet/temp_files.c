// Selecing layer temp files


///////////////////////////////////////////////////// CONV 1 ///////////////////////////////////////////////
if (in_data[0].size() == 154587 && out_data[0].size() == 290400 && bias.size() == 96 && W.size() == 34848) {
// ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
// ./write_array.py 121 3 96 weight_1 transfer_files/FILTER_DATA.dat param_headers/1_weight.h 
// ./write_array.py 1 1 96 bias_1 transfer_files/BIAS_DATA.dat param_headers/1_bias.h

///////////////////////////////////////////////////// CONV 2 ///////////////////////////////////////////////
if (in_data[0].size() == 92256 && out_data[0].size() == 186624 && bias.size() == 256 && W.size() == 614400) {
// ./write_array.py 31 31 96 in_2 transfer_files/IN_DATA.dat param_headers/2_in.h
// ./write_array.py 25 96 256 weight_2 transfer_files/FILTER_DATA.dat param_headers/2_weight.h 
// ./write_array.py 1 1 256 bias_2 transfer_files/BIAS_DATA.dat param_headers/2_bias.h


///////////////////////////////////////////////////// CONV 3 ///////////////////////////////////////////////
if (in_data[0].size() == 57600 && out_data[0].size() == 64896 && bias.size() == 384 && W.size() == 884736) {
// ./write_array.py 15 15 256 in_3 transfer_files/IN_DATA.dat param_headers/3_in.h
// ./write_array.py 9 256 384 weight_3 transfer_files/FILTER_DATA.dat param_headers/3_weight.h 
// ./write_array.py 1 1 384 bias_3 transfer_files/BIAS_DATA.dat param_headers/3_bias.h


///////////////////////////////////////////////////// CONV 4 ///////////////////////////////////////////////
if (in_data[0].size() == 86400 && out_data[0].size() == 64896 && bias.size() == 384 && W.size() == 1327104) {
// ./write_array.py 15 15 384 in_4 transfer_files/IN_DATA.dat param_headers/4_in.h
// ./write_array.py 9 384 384 weight_4 transfer_files/FILTER_DATA.dat param_headers/4_weight.h 
// ./write_array.py 1 1 384 bias_4 transfer_files/BIAS_DATA.dat param_headers/4_bias.h


///////////////////////////////////////////////////// CONV 5 ///////////////////////////////////////////////
if (in_data[0].size() == 86400 && out_data[0].size() == 43264 && bias.size() == 256 && W.size() == 884736) {
// ./write_array.py 15 15 384 in_5 transfer_files/IN_DATA.dat param_headers/5_in.h
// ./write_array.py 9 384 256 weight_5 transfer_files/FILTER_DATA.dat param_headers/5_weight.h 
// ./write_array.py 1 1 256 bias_5 transfer_files/BIAS_DATA.dat param_headers/5_bias.h


/////////////////////////////////////////////////////  FC 6  ///////////////////////////////////////////////
if (in_data[0].size() == 9216 && out_data[0].size() == 4096 && bias.size() == 4096 && W.size() == 37748736) {
// ./write_array.py 1 1 9216 in_6 transfer_files/IN_DATA.dat param_headers/6_in.h
// ./write_array.py 1 1 37748736 weight_6 transfer_files/FILTER_DATA.dat param_headers/6_weight.h 
// ./write_array.py 1 1 4096 bias_6 transfer_files/BIAS_DATA.dat param_headers/6_bias.h


/////////////////////////////////////////////////////  FC 7  ///////////////////////////////////////////////
if (in_data[0].size() == 4096 && out_data[0].size() == 4096 && bias.size() == 4096 && W.size() == 16777216) {
// ./write_array.py 1 1 4096 in_7 transfer_files/IN_DATA.dat param_headers/7_in.h
// ./write_array.py 1 1 16777216 weight_7 transfer_files/FILTER_DATA.dat param_headers/7_weight.h 
// ./write_array.py 1 1 4096 bias_7 transfer_files/BIAS_DATA.dat param_headers/7_bias.h


/////////////////////////////////////////////////////  FC 8  ///////////////////////////////////////////////
if (in_data[0].size() == 4096 && out_data[0].size() == 1000 && bias.size() == 1000 && W.size() == 4096000) {
// ./write_array.py 1 1 4096 in_8 transfer_files/IN_DATA.dat param_headers/8_in.h
// ./write_array.py 1 1 4096000 weight_8 transfer_files/FILTER_DATA.dat param_headers/8_weight.h 
// ./write_array.py 1 1 1000 bias_8 transfer_files/BIAS_DATA.dat param_headers/8_bias.h


