#!/bin/bash


# amplifier.jpg  cat.jpg        electricGuitar.jpg  laserPrinter.jpg  phone.jpg  slippers.jpg
# ball.jpg       cellphone.jpg  inkjetPrinter.jpg   monitor.jpg       sink.jpg

./alexnet tiny-model-alexnet classes.txt imagens/slippers.jpg out_data/c/NET_OUT_slippers.dat > term_outputs/tiny_c_slippers.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_slippers.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_slippers.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_slippers.dat

./alexnet tiny-model-alexnet classes.txt imagens/amplifier.jpg out_data/c/NET_OUT_amplifier.dat > term_outputs/tiny_c_amplifier.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_amplifier.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_amplifier.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_amplifier.dat

./alexnet tiny-model-alexnet classes.txt imagens/cat.jpg out_data/c/NET_OUT_cat.dat > term_outputs/tiny_c_cat.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_cat.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_cat.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_cat.dat

./alexnet tiny-model-alexnet classes.txt imagens/ball.jpg out_data/c/NET_OUT_ball.dat > term_outputs/tiny_c_ball.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_ball.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_ball.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_ball.dat

./alexnet tiny-model-alexnet classes.txt imagens/cellphone.jpg out_data/c/NET_OUT_cellphone.dat > term_outputs/tiny_c_cellphone.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_cellphone.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_cellphone.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_cellphone.dat

./alexnet tiny-model-alexnet classes.txt imagens/electricGuitar.jpg out_data/c/NET_OUT_electricGuitar.dat > term_outputs/tiny_c_electricGuitar.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_electricGuitar.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_electricGuitar.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_electricGuitar.dat

./alexnet tiny-model-alexnet classes.txt imagens/inkjetPrinter.jpg out_data/c/NET_OUT_inkjetPrinter.dat > term_outputs/tiny_c_inkjetPrinter.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_inkjetPrinter.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_inkjetPrinter.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_inkjetPrinter.dat

./alexnet tiny-model-alexnet classes.txt imagens/laserPrinter.jpg out_data/c/NET_OUT_laserPrinter.dat > term_outputs/tiny_c_laserPrinter.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_laserPrinter.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_laserPrinter.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_laserPrinter.dat

./alexnet tiny-model-alexnet classes.txt imagens/monitor.jpg out_data/c/NET_OUT_monitor.dat > term_outputs/tiny_c_monitor.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_monitor.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_monitor.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_monitor.dat

./alexnet tiny-model-alexnet classes.txt imagens/phone.jpg out_data/c/NET_OUT_phone.dat > term_outputs/tiny_c_phone.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_phone.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_phone.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_phone.dat

./alexnet tiny-model-alexnet classes.txt imagens/sink.jpg out_data/c/NET_OUT_sink.dat > term_outputs/tiny_c_sink.txt
# ./write_array.py 227 227 3 in_1 transfer_files/IN_DATA.dat param_headers/1_in.h
# ./net > term_outputs/c_sink.txt
# mv transfer_files/OUT_DATA.dat out_data/tiny/OUT_DATA_sink.dat
# mv transfer_files/NET_OUT.dat out_data/c/NET_OUT_sink.dat
