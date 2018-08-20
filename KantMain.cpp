
#include "stdafx.h"
#include <Windows.h>

#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <process.h>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;


bool cmd_manual = false;
bool cmd_quit = false;
bool cmd_window = true;
bool cmd_cut = false;
bool cmd_fps = true;
int cmd_per_second = 5;

unsigned int _stdcall thread_cmd(void* data)
{
	printf("threadUser %d \n", GetCurrentThreadId());
	
	string cmd;
	while (cmd_quit != true) {
		cout << "5, number type, get 1 picture per 5 sec" << endl;
		cout << "q, quit" << endl;
		cout << "m, manual operate, please press c to get picture" << endl;
		cout << "c, get 1 picture by key press" << endl;
		cout << "o, open the instant window" << endl;
		cout << "s, shutdown the instant window" << endl;
		cout << "f, switch on/off fps info" << endl << endl;
		cin >> cmd;
		std::istringstream iss(cmd);
		if (cmd == "q") {
			cmd_quit = true;
			cout << "#q, quit" << endl << endl;
			break;
		}
		else if (cmd == "m") {
			cmd_manual = true;
			cout << "#m, manual operate, please press c to get picture" << endl << endl;
		}
		else if (cmd == "c") {
			cout << "#c, get 1 picture by key press" << endl;
			cmd_cut = true;
		}
		else if (cmd == "o") {
			cmd_window = true;
			cout << "#o, open the instant window" << endl << endl;
		}
		else if (cmd == "s") {
			cmd_window = false;
			cout << "#s, shutdown the instant window" << endl << endl;
		}
		else if (cmd == "f") {
			cmd_fps = !cmd_fps;
			if (cmd_fps) {
				cmd_fps = false;
				cout << "#turn off the fps info" << endl << endl;
			}
			else {
				cmd_fps = false;
				cout << "#turn on the fps info" << endl << endl;
			}
		}
		else if (!(iss >> cmd_per_second).fail()) {
			cmd_manual = false;
			cout << "## auto get 1 picture per " << cmd_per_second << " sec" << endl << endl;
		}
		else {
			cout << "Unkonwn cmd: "<< cmd << endl << endl;
		}

		if (cmd_manual == false) {
			cout << "	## Default: auto get 1 picture per " << cmd_per_second << " sec" << endl << endl;
		}
	}
	cout << "System is going to shutdown soon..." << endl;
	cout << "Please press any key to quit!" << endl;
	getchar();
	return 0;
}

int img_count = 100;

void saveImage(Mat &src) {
	stringstream ss;
	ss << img_count++;
	string file_name = "img_" + ss.str() + ".jpg";
	cout << "file_name = " << file_name << endl;
	imwrite(file_name, src);
}

int main()
{
	VideoCapture cap;
	string msg;
	cap = VideoCapture(CV_CAP_DSHOW);	//
	cap.open(0);						//
	if (!cap.isOpened()){
		cout << "Can't find USB Cam" << endl;
		getchar();
		return -1;
	}
	cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));//设置为MJPG格式
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920/2);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1084 / 2);
	TickMeter tm;
	HANDLE handle_cmd, handle_timer;
	handle_cmd = (HANDLE)_beginthreadex(0, 0, &thread_cmd, 0, 0, 0);
	clock_t t_start = clock();
	while (1){
		if (cmd_quit) {
			break;
		}
		//
		tm.reset();
		tm.start();
		Mat frame;
		for (int i = 0; i < 100; i++){
			cap >> frame;
			if (cmd_window) {
				imshow("frame", frame);
			}
			else {
				destroyAllWindows();
			}
			if (waitKey(1) == 27) {}
			// cut picuure
			if (cmd_manual == false) {
				int cost = (clock() - t_start) / CLOCKS_PER_SEC;
				if (cost > cmd_per_second) {
					cout << "   $$截图操作$$$$...." << cost << endl;
					saveImage(frame);
					t_start = clock();
				}
			}
			else {
				if (cmd_cut) {
					cout << "   $$Manual-->截图操作$$$$...." << endl;
					saveImage(frame);
					cmd_cut = false;
				}
			}
		}
		//
		tm.stop();
		if (cmd_fps) {
			cout << "                              " << 100 / tm.getTimeSec() << "fps" << endl;//输出帧率
		}
		
	}
	
	WaitForSingleObject(handle_cmd, INFINITE);
	CloseHandle(handle_cmd);
	return 0;
}



