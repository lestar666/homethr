#include <iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

float distance(float * l, float * m, int n) {
	int totalnum = 0;
	for (int i = 0; i < n; i++) {
		totalnum = totalnum + pow((*(l + i) - *(m + i)), 2);
	}
	return totalnum;
}

void hist(Mat & src, int cell_Size, int nX, int nY, float * hist) {
	Mat gx, gy;
	Mat mag, angle;
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);
	cartToPolar(gx, gy, mag, angle, true);
	int n = 0;
	for (int i = 0; i < nY; i++) {

		for (int j = 0; j < nX; j++) {

			int n_x = j * cell_Size, n_y = i * cell_Size;

			for (int l = n_x; l < cell_Size + n_x; l++) {

				for (int m = n_y; m < cell_Size + n_y; m++) {

					int region = (angle.at<float>(l, m)) / 40;
					*(hist + region + n) += mag.at<float>(l, m);
				}
			}
			n += 9;
		}
	}
}

int main() {

	Mat srcMat = imread("template.png", 0);
	Mat result = imread("img.png", 0);
	if (!srcMat.data) {
		cout << "fail" << endl;
	}if (!result.data) {
		cout << "fail2" << endl;
	}

	int cellsize = 16;
	int nX = srcMat.cols / cellsize;
	int nY = srcMat.rows / cellsize;
	int bins = 9*nX*nY;

	double minR = 1e30;
	int minX;
	int minY;
	int height = result.rows;
	int width = result.cols;
	int nheight = srcMat.rows;
	int nwidth = srcMat.cols;
	
	float * rhist = new float[bins];
    memset(rhist, 0, sizeof(float)*bins);

	hist(srcMat, cellsize, nX, nY, rhist);

	for (int i = 0; i < height - nheight + 1; i++){

		for (int j = 0; j < width - nwidth + 1; ){

			Rect rect(j, i, nwidth, nheight);
			Mat iroi = result(rect);
			float * roi_hist = new float[bins];
			memset(roi_hist, 0, sizeof(float)*bins);
			hist(iroi, cellsize, nX, nY, roi_hist);

			double dis = distance(rhist, roi_hist, bins);

			if (dis < minR)
			{
				minR = dis;
				minX = j;
				minY = i;
			}
			delete[]  roi_hist;
			j = j + 5;
		}
		i = i + 5;
	}
	delete[] rhist;
	Rect rect(minX, minY, nwidth, nheight);
	rectangle(result, rect, Scalar(255, 255, 255), 2, 8, 0);
	imshow("result", result);
	waitKey(0);
	return 0;
}

