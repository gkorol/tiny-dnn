#include <iostream>
#include "tiny_dnn/tiny_dnn.h"
#include <stdio.h>

using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace std;

// rescale output to 0-100
template <typename Activation>
double rescale(double x) {
  Activation a(1);
  return 100.0 * (x - a.scale().first) / (a.scale().second - a.scale().first);
}

void convert_image(const std::string &imagefilename,
                   int w,
                   int h,
                   vec_t &data) {
  image<> img(imagefilename, image_type::bgr);
  image<> resized = resize_image(img, w, h);
  data.resize(resized.width() * resized.height() * resized.depth());
  float imageNetMeans[] = {103.939, 116.779, 123.68};  // BGR
  for (size_t c = 0; c < resized.depth(); ++c) {
    for (size_t y = 0; y < resized.height(); ++y) {
      for (size_t x = 0; x < resized.width(); ++x) {
        data[c * resized.width() * resized.height() + y * resized.width() + x] =
          resized[y * resized.width() + x + c] - imageNetMeans[c];
      }
    }
  }
}

void recognize(const std::string &model_name,
               const std::string &class_labels_file_name,
               const std::string &src_filename,
               const std::string &src_outdata) {
  network<sequential> nn;

  // Korol
  #ifdef PRINT_DEBUG
    cout << "[Main/recognize] Loading Model" << endl;
  #endif
  // Load the model
  nn.load(model_name);


  // Korol
  #ifdef PRINT_DEBUG
  cout << "[Main/recognize] Converting image" << endl;
  #endif
  // convert imagefile to vec_t
  vec_t data;
  convert_image(src_filename, 227, 227, data);

  // recognize
  // Korol
  #ifdef PRINT_DEBUG
  cout << "[Main/recognize] Calling predict method" << endl;
  #endif
  // auto res = nn.predict(data);
  vector<double> res;
  std::ifstream fin_outdata;
  fin_outdata.open(src_outdata, std::ios::in);
  std::string l;
  while (std::getline(fin_outdata, l)) {
    std::istringstream iss(l);
    double a;
    if (!(iss >> a)) { break; } // error

    res.push_back(a);
  }
  fin_outdata.close();

  vector<pair<double, int>> scores;

  // std::ofstream fout_outdata;
  // fout_outdata.open("transfer_files/OUT_DATA.dat", std::ios::out | std::ios::trunc);
  // for( int i=0; i < 1000; i++) {
  //  fout_outdata << std::fixed << res[i] << endl;
  // }
  // fout_outdata.close();

  // Load class labels
  ifstream class_labels_file(class_labels_file_name, ios::in);
  string line;
  vector<string> class_labels;
  while (getline(class_labels_file, line)) {
    class_labels.push_back(line);
  }

  // sort & print top-5
  for (int i = 0; i < 1000; i++) {
    scores.emplace_back(rescale<tanh_layer>(res[i]), i);
    // cout<<"< "<<scores[i].first<<" , "<<scores[i].second<<" >"<<endl;
  }

  sort(scores.begin(), scores.end(), greater<pair<double, int>>());

  for (int i = 0; i < 5; i++) {
    cout << "Predicted class: " << class_labels[scores[i].second] << " ("
         << scores[i].second << ") | Confidence: " << scores[i].first << " %"
         << endl;
  }
}

int main(int argc, char **argv) {
  // if (argc != 4) {
  if (argc != 5) {
    cout << "Usage: " << argv[0]
         << " <Model file> <Classes label file> <Input image file>" << endl;
    return 0;
  }
  // recognize(argv[1], argv[2], argv[3]);
  recognize(argv[1], argv[2], argv[3], argv[4]);
}
