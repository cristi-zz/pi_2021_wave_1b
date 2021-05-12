// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>


void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				Vec3b v3 = src(i,j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i,j) = (r+g+b)/3;
			}
		}
		
		imshow("original image",src);
		imshow("gray image",dst);
		waitKey();
	}
}

std::vector<int> computeLowVector(std::vector<int> input) {
	std::vector<int> vector_low(input.size() / 2);

	for (int i = 0; i < input.size() / 2; i++)
		vector_low[i] = (input[i * 2] + input[i * 2 + 1]) / 2;

	return vector_low;
}

std::vector<int> computeHighVector(std::vector<int> input) {
	std::vector<int> vector_high(input.size() / 2);

	for (int i = 0; i < input.size() / 2; i++)
		vector_high[i] = (input[i * 2] - input[i * 2 + 1]) / 2;

	return vector_high;
}

void oneDConstruction(std::vector<int> input) {

	std::vector<int> vector_low = computeLowVector(input);
	std::vector<int> vector_high = computeHighVector(input);

	for (int i = 0; i < input.size(); i++)
		printf("%d ", input[i]);
	printf("\n");


	for (int i = 0; i < input.size()/2; i++)
		printf("%d ", vector_low[i]);

	for (int i = 0; i < input.size() / 2; i++)
		printf("%d ", vector_high[i]);
	system("pause");
}

std::vector<int> computeHighUpSample(std::vector<int> input) {
	std::vector<int> high_upSample(input.size()*2);
	std::vector<int> h{ 1,-1 };

	for (int i = 0; i < input.size() * 2; i++)
		high_upSample[i] = input[i / 2] * h[i % 2];

	return high_upSample;
}

std::vector<int> computeLowUpSample(std::vector<int> input) {
	std::vector<int> low_upSample(input.size() * 2);
	std::vector<int> h{ 1,-1 };

	for (int i = 0; i < input.size() * 2; i++)
		low_upSample[i] = input[i / 2];

	return low_upSample;
}

void oneDDeconstruction(std::vector<int> vector_low, std::vector<int> vector_high) {

	std::vector<int> low_upSample = computeLowUpSample(vector_low);
	std::vector<int> high_upSample = computeHighUpSample(vector_high);
	std::vector<int> upsample_signal(low_upSample.size());

	for (int i = 0; i < upsample_signal.size(); i++)
		upsample_signal[i] = high_upSample[i] + low_upSample[i];

	for (int i = 0; i < vector_low.size(); i++)
		printf("%d ", vector_low[i]);

	for (int i = 0; i < vector_high.size(); i++)
		printf("%d ", vector_high[i]);

	printf("\n");

	for (int i = 0; i < low_upSample.size(); i++)
		printf("%d ", low_upSample[i]);

	printf("\n");

	for (int i = 0; i < high_upSample.size(); i++)
		printf("%d ", high_upSample[i]);

	printf("\n");

	for (int i = 0; i < upsample_signal.size(); i++)
		printf("%d ", upsample_signal[i]);
	system("pause");
}

Mat_<Vec3b> twoDConstruction(Mat_<Vec3b> src, int level) {
	int height = src.rows;
	int width = src.cols;
	Mat_<Vec3b> interm = src.clone();
	Mat_<Vec3b> dst(height, width);

	int currLevel = 1;
	int levelHeight = height;
	int levelWidth = width;
	while (currLevel <= level) {
		for (int i = 0; i < levelHeight - 1; i += 2)
		{
			for (int j = 0; j < levelWidth - 1; j += 2)
			{
				dst(i / 2, j / 2) = interm(i, j);
				dst(i / 2, j / 2 + levelWidth / 2) = interm(i, j + 1);
				dst(i / 2 + levelHeight / 2, j / 2) = interm(i + 1, j);
				dst(i / 2 + levelHeight / 2, j / 2 + levelWidth / 2) = interm(i + 1, j + 1);
			}
		}
		interm = dst.clone();
		currLevel++;
		levelHeight /= 2;
		levelWidth /= 2;
	}
	return dst;
}

Mat_<Vec3b> twoDDeconstruction(Mat_<Vec3b> src, int level) {
	int height = src.rows;
	int width = src.cols;
	Mat_<Vec3b> interm = src.clone();
	Mat_<Vec3b> dst = src.clone();

	int depth = pow(2, level);
	int levelHeight = height / depth;
	int levelWidth = width / depth;
	while (1 <= level) {
		for (int i = 0; i < levelHeight * 2 - 1; i+= 2)
		{
			for (int j = 0; j < levelWidth * 2 - 1; j+= 2)
			{
				dst(i, j) = interm(i / 2 , j / 2);
				dst(i, j + 1) = interm(i / 2, j / 2 + levelWidth);
				dst(i + 1, j) = interm(i / 2 + levelHeight, j / 2);
				dst(i + 1, j + 1) = interm(i / 2 + levelHeight, j / 2 + levelWidth);
			}
		}
		interm = dst.clone();
		level--;
		levelHeight *= 2;
		levelWidth *= 2;
	}
	return dst;
}

float meanAbsoluteError(Mat_<uchar> originalImg, Mat_<uchar> reconstructedImg) {
	float mean = 0.0f;

	for (int i = 0; i < originalImg.rows; i++)
		for (int j = 0; j < originalImg.cols; j++)
			mean += abs(originalImg(i,j) - reconstructedImg(i, j));

	return (float)mean / (originalImg.rows * originalImg.cols);
}


Mat_<int> imageLL(Mat_<int> dstLow) {

	//LL
	Mat_<int> dstLL(dstLow.rows / 2, dstLow.cols);
	std::vector<int> colAux(dstLow.rows);
	for (int i = 0; i < dstLow.cols; i++) {
		for (int a = 0; a < dstLow.rows; a++) {
			colAux.at(a) = dstLow(a, i);
		}
		std::vector<int> vector_low = computeLowVector(colAux);
		int k = 0;
		for (int j = 0; j < dstLow.rows / 2; j++) {
			dstLL(j, i) = vector_low.at(j);
		}
	}
	return dstLL;
}



Mat_<int> imageLH(Mat_<int> dstLow) {

	//LH
	Mat_<int> dstLH(dstLow.rows / 2, dstLow.cols);
	std::vector<int> colAux(dstLow.rows);
	for (int i = 0; i < dstLow.cols; i++) {
		for (int a = 0; a < dstLow.rows; a++) {
			colAux.at(a) = dstLow(a, i);
		}
		std::vector<int> vector_high = computeHighVector(colAux);
		int k = 0;
		for (int j = 0; j < dstLow.rows / 2; j++) {
			dstLH(j, i) = vector_high.at(j) + 128;
		}
	}
	return dstLH;
}

Mat_<int> imageHL(Mat_<int> dstHigh) {
	//HL
	Mat_<int> dstHL(dstHigh.rows / 2, dstHigh.cols);
	std::vector<int> colAux(dstHigh.rows);
	for (int i = 0; i < dstHigh.cols; i++) {
		for (int a = 0; a < dstHigh.rows; a++) {
			colAux.at(a) = dstHigh(a, i);
		}
		std::vector<int> vector_low = computeLowVector(colAux);
		int k = 0;
		for (int j = 0; j < dstHigh.rows / 2; j++) {
			dstHL(j, i) = vector_low.at(j);
		}
	}
	return dstHL;
}



Mat_<int> imageHH(Mat_<int> dstHigh) {
	//HH
	Mat_<int> dstHH(dstHigh.rows / 2, dstHigh.cols);
	std::vector<int> colAux(dstHigh.rows);
	for (int i = 0; i < dstHigh.cols; i++) {
		for (int a = 0; a < dstHigh.rows; a++) {
			colAux.at(a) = dstHigh(a, i);
		}
		std::vector<int> vector_high = computeHighVector(colAux);
		int k = 0;
		for (int j = 0; j < dstHigh.rows / 2; j++) {
			dstHH(j, i) = vector_high.at(j) + 128;
		}
	}
	return dstHH;
}

Mat_<int> imageLow(Mat_<int>src) {

	//low
	Mat_<int> dstLow(src.rows, src.cols / 2);
	std::vector<int> rowAux(src.rows);
	for (int i = 0; i < src.rows; i++) {
		for (int a = 0; a < src.cols; a++) {
			rowAux.at(a) = src(i, a);
		}
		std::vector<int> vector_low = computeLowVector(rowAux);
		int k = 0;
		for (int j = 0; j < src.rows / 2; j++) {
			dstLow(i, j) = vector_low.at(j);
		}
	}
	return dstLow;
}
Mat_<int> imageHigh(Mat_<int>src) {

	//high
	Mat_<int> dstHigh(src.rows, src.cols / 2);
	std::vector<int> rowAux(src.rows);
	for (int i = 0; i < src.rows; i++) {
		for (int a = 0; a < src.cols; a++) {
			rowAux.at(a) = src(i, a);
		}
		std::vector<int> vector_high = computeHighVector(rowAux);
		int k = 0;
		for (int j = 0; j < src.cols / 2; j++) {
			dstHigh(i, j) = vector_high.at(j) + 128;
		}
	}
	return dstHigh;
}

void twoDD(int level) {
	char fname[MAX_PATH];
	openFileDlg(fname);
	Mat_<int> src;
	src = imread(fname, IMREAD_GRAYSCALE);
	Mat_<int> dstLow(src.rows, src.cols / 2);
	Mat_<int> dstHigh(src.rows, src.cols / 2);

	std::vector<int> rowAux(src.rows);
	std::vector<int> colAux(src.cols);

	int height = src.rows;
	int width = src.cols;

	printf("%d %d\n", height, width);

	while (level) {


		//low
		dstLow = imageLow(src);
		imshow("L", (Mat_<uchar>) dstLow);
		resizeWindow("L", src.rows, src.cols);

		//high
		dstHigh = imageHigh(src);
		imshow("H", (Mat_<uchar>) dstHigh);
		resizeWindow("H", src.rows, src.cols);

		//LL
		Mat_<int> dstLL(height / 2, width / 2);
		dstLL = imageLL(dstLow);
		imshow("LL", (Mat_<uchar>) dstLL);
		resizeWindow("LL", height, width);

		//LH
		Mat_<int> dstLH(height / 2, width / 2);
		dstLH = imageLH(dstLow);
		imshow("LH", (Mat_<uchar>) dstLH);
		resizeWindow("LH", height, width);

		//HL
		Mat_<int> dstHL(height / 2, width / 2);
		dstHL = imageHL(dstHigh);
		imshow("HL", (Mat_<uchar>) dstHL);
		resizeWindow("HL", height, width);

		//HH
		Mat_<int> dstHH(height / 2, width / 2);
		dstHH = imageHH(dstHigh);
		imshow("HH", (Mat_<uchar>) dstHH);
		resizeWindow("HH", height, width);

		level--;
	}

	imshow("src", (Mat_<uchar>) src);
	waitKey(0);
}

int main()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Basic image opening...\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Color to Gray\n");
		printf(" 4 - 2D Construction\n");
		printf(" 5 - 2D Deconstruction\n");
		printf(" 6 - 1D Construction\n");
		printf(" 7 - 1D Deconstruction\n");
		printf(" 8 - 2D Deconstruction\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		switch (op)
		{
			case 1:
				testOpenImage();
				break;
			case 2:
				testOpenImagesFld();
				break;
			case 3:
				testColor2Gray();
				break;
			case 4: {
				char fname[MAX_PATH];
				if (openFileDlg(fname))
				{
					Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);
					int level;
					if (scanf("%d", &level) == 1) {
						Mat_<Vec3b> dst = twoDConstruction(src, level);
						imshow("original image", src);
						imshow("construction image", dst);
						waitKey();
					}
					else {
						printf("Invalid file image\n");
					}
				}
				break;
			}
			case 5: {
				char fname[MAX_PATH];
				if (openFileDlg(fname))
				{
					Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);
					int level;
					if (scanf("%d", &level) == 1) {
						Mat_<Vec3b> interm = twoDConstruction(src, level);
						Mat_<Vec3b> dst = twoDDeconstruction(interm, level);
						imshow("original image", src);
						imshow("constructed image", interm);
						imshow("deconstruction image", dst);
						waitKey();
					}
					else {
						printf("Invalid file image\n");
					}
				}
				break;
			}
			case 6:{
				oneDConstruction({ 9, 7, 3, 5, 6, 10, 2, 6 });
				break;
			}
			case 7: {
				std::vector<int> vector_low = computeLowVector({ 9, 7, 3, 5, 6, 10, 2, 6 });
				std::vector<int> vector_high = computeHighVector({ 9, 7, 3, 5, 6, 10, 2, 6 });

				oneDDeconstruction(vector_low, vector_high);
				break;
			}
			case 8: {

				twoDD(1);
				break;
			}
		
		}
	}
	while (op!=0);
	return 0;
}
