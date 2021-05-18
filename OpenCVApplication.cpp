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

//************ 1D Construction

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

//************ 1D Decosntruction

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

/*
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
*/

//************ 2D Decosntruction

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

void twoDDeconstruct(Mat_<int> src, Mat_<int> &dstLL, Mat_<int> &dstLH, Mat_<int> &dstHL, Mat_<int> &dstHH){

	Mat_<int> dstLow(src.rows, src.cols / 2);
	Mat_<int> dstHigh(src.rows, src.cols / 2);

	dstLow = imageLow(src);
	dstHigh = imageHigh(src);

	dstLL = imageLL(dstLow);
	dstLH = imageLH(dstLow);
	dstHL = imageHL(dstHigh);
	dstHH = imageHH(dstHigh);

}

void twoDDeconstructShow() {

	char fname[MAX_PATH];
	if (openFileDlg(fname)) {
		Mat_<int> src;
		src = imread(fname, IMREAD_GRAYSCALE);
		imshow("src", (Mat_<uchar>) src);

		Mat_<int> dstLow(src.rows, src.cols / 2);
		Mat_<int> dstHigh(src.rows, src.cols / 2);

		int height = src.rows;
		int width = src.cols;

		printf("%d %d\n", height, width);

		// we compute the lowImage and highImage just for the purpose of displaying them

		//lowImage
		dstLow = imageLow(src);
		imshow("L", (Mat_<uchar>) dstLow);
		resizeWindow("L", src.rows, src.cols);

		//highImage
		dstHigh = imageHigh(src);
		imshow("H", (Mat_<uchar>) dstHigh);
		resizeWindow("H", src.rows, src.cols);


		Mat_<int> dstLL(height / 2, width / 2);
		Mat_<int> dstLH(height / 2, width / 2);
		Mat_<int> dstHL(height / 2, width / 2);
		Mat_<int> dstHH(height / 2, width / 2);

		twoDDeconstruct(src, dstLL, dstLH, dstHL, dstHH);

		imshow("LL", (Mat_<uchar>) dstLL);
		resizeWindow("LL", height, width);

		imshow("LH", (Mat_<uchar>) dstLH);
		resizeWindow("LH", height, width);

		imshow("HL", (Mat_<uchar>) dstHL);
		resizeWindow("HL", height, width);

		imshow("HH", (Mat_<uchar>) dstHH);
		resizeWindow("HH", height, width);

		waitKey(0);
	}
}

//************ 2D Decosntruction Recursive

std::vector<Mat_<int>> twoDDeconstructRecursive(Mat_<int> src, int level) {

	std::vector<Mat_<int>> dest;

	Mat_<int> startingImg = src.clone();

	while (level) {
		Mat_<int> dstLL(startingImg.rows / 2, startingImg.cols / 2);
		Mat_<int> dstLH(startingImg.rows / 2, startingImg.cols / 2);
		Mat_<int> dstHL(startingImg.rows / 2, startingImg.cols / 2);
		Mat_<int> dstHH(startingImg.rows / 2, startingImg.cols / 2);

		twoDDeconstruct(startingImg, dstLL, dstLH, dstHL, dstHH);

		dest.push_back(dstHH);
		dest.push_back(dstLH);
		dest.push_back(dstHL);

		startingImg = dstLL.clone();
		level--;
	}
	dest.push_back(startingImg);

	return dest;
}

void twoDDeconstructRecursiveShow() {

	char fname[MAX_PATH];
	if (openFileDlg(fname)) {
		Mat_<int> src=imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;

		int level = 0;
		printf("Level=");
		scanf("%d", &level);

		imshow("src", (Mat_<uchar>) src);
		
		std::vector<Mat_<int>> dest=twoDDeconstructRecursive(src, level);

		level = 1;
		for (int i = 0; i < dest.size()-1; i+=3) {
			
			String hh = "HH" + std::to_string(level);
			imshow(hh, (Mat_<uchar>) dest[i]);
			resizeWindow(hh, height, width);

			String hl = "HL" + std::to_string(level);
			imshow(hl, (Mat_<uchar>) dest[i + 1]);
			resizeWindow(hl, height, width);

			String lh = "LH" + std::to_string(level);
			imshow(lh, (Mat_<uchar>) dest[i + 2]);
			resizeWindow(lh, height, width);

			level++;
		}

		imshow("LL", (Mat_<uchar>) dest[dest.size()-1]);
		resizeWindow("LL", height, width);
	
		waitKey(0);
	}
	
}
//************ MAE

float meanAbsoluteError(Mat_<uchar> originalImg, Mat_<uchar> reconstructedImg) {
	float mean = 0.0f;

	for (int i = 0; i < originalImg.rows; i++)
		for (int j = 0; j < originalImg.cols; j++)
			mean += abs(originalImg(i, j) - reconstructedImg(i, j));


	return (float)mean / (originalImg.rows * originalImg.cols);
}

//************ 2D Construction Recursive


void computeLowHigh(Mat_<int> imgLL, Mat_<int> imgLH, Mat_<int> imgHL, Mat_<int> imgHH, Mat_<int>& dstL, Mat_<int>& dstH) {

	// we compute the lowImage and highImage from the images LL,LH,HL,HH
	int heightLL = imgLL.rows;

	for (int i = 0; i < heightLL; i++) {

		std::vector<int> vector_lowL(heightLL);
		std::vector<int> vector_highL(heightLL);
		std::vector<int> vector_lowH(heightLL);
		std::vector<int> vector_highH(heightLL);

		for (int j = 0; j < heightLL; j++) {
			vector_lowL.at(j) = imgLL(j, i);
			vector_highL.at(j) = imgLH(j, i) - 128;
			vector_lowH.at(j) = imgHL(j, i);
			vector_highH.at(j) = imgHH(j, i) - 128;
		}

		std::vector<int> low_upSample = computeLowUpSample(vector_lowL);
		std::vector<int> high_upSample = computeHighUpSample(vector_highL);
		std::vector<int> upsample_signalL(low_upSample.size());

		for (int j = 0; j < upsample_signalL.size(); j++)
			upsample_signalL[j] = high_upSample[j] + low_upSample[j];

		for (int j = 0; j < heightLL * 2; j++) {
			dstL(j, i) = upsample_signalL.at(j);
		}


		low_upSample = computeLowUpSample(vector_lowH);
		high_upSample = computeHighUpSample(vector_highH);
		std::vector<int> upsample_signalH(low_upSample.size());

		for (int j = 0; j < upsample_signalL.size(); j++)
			upsample_signalH[j] = high_upSample[j] + low_upSample[j];

		for (int j = 0; j < heightLL * 2; j++) {
			dstH(j, i) = upsample_signalH.at(j);
		}

	}

}

void computeImageFromLowHigh(int height, int width, Mat_<int> dstL, Mat_<int> dstH, Mat_<int>& dstInterm) {

	// we compute the image from the lowImage and highImage

	for (int i = 0; i < height; i++) {
		std::vector<int> vector_lowL(height * 2);
		std::vector<int> vector_highL(height * 2);
		
		for (int j = 0; j < height/2; j++) {
			
			vector_lowL.at(j) = dstL(i, j);
			vector_highL.at(j) = dstH(i, j) - 128;
		}
		
		std::vector<int> low_upSample = computeLowUpSample(vector_lowL);
		std::vector<int> high_upSample = computeHighUpSample(vector_highL);
		std::vector<int> upsample_signalL(low_upSample.size());
		for (int j = 0; j < upsample_signalL.size(); j++)
			upsample_signalL[j] = high_upSample[j] + low_upSample[j];
		for (int j = 0; j < width; j++) {
			dstInterm(i, j) = upsample_signalL.at(j);
		}

	}
}

Mat_<int> twoDConstructRecursive(std::vector<Mat_<int>> images) {

	int i = images.size();
	Mat_<int> imgLL = images[--i];
	//Mat_<int> srcFinal();

	while (i / 3) {

		int heightLL = imgLL.rows;
		int widthLL = imgLL.cols;

		Mat_<int> imgHL = images[--i];
		Mat_<int> imgLH = images[--i];
		Mat_<int> imgHH = images[--i];

		Mat_<int> dstInterm(heightLL * 2, widthLL * 2);

		Mat_<int> dstH(heightLL * 2, widthLL);
		Mat_<int> dstL(heightLL * 2, widthLL);
		int height = heightLL * 2;
		int width = widthLL * 2;
		computeLowHigh(imgLL, imgLH, imgHL, imgHH, dstL, dstH);

		computeImageFromLowHigh(height, width, dstL, dstH, dstInterm);

		imgLL = dstInterm.clone();
	}

	Mat_<int> dstFinal = imgLL.clone();
	return dstFinal;

}

Mat_<int> scaling(Mat_<int> originalImg, Mat_<int> reconstructedImg) {

	Mat_<int> difference(originalImg.rows, originalImg.cols, 0);
	for (int i=0; i<originalImg.rows; i++)
		for (int j = 0; j < originalImg.cols; j++)
			if (reconstructedImg(i, j) != originalImg(i, j)) {
				difference(i, j) = (originalImg(i, j) - reconstructedImg(i, j)) * 10;
			}

	return difference;
}

void twoDConstructRecursiveShow() {

	char fname[MAX_PATH];
	if (openFileDlg(fname)) {
		Mat_<int> src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;

		int level = 0;
		printf("Level=");
		scanf("%d", &level);

		imshow("src", (Mat_<uchar>) src);

		std::vector<Mat_<int>> dest = twoDDeconstructRecursive(src, level);

		level = 1;
		for (int i = 0; i < dest.size() - 1; i += 3) {

			String hh = "HH" + std::to_string(level);
			imshow(hh, (Mat_<uchar>) dest[i]);
			resizeWindow(hh, height, width);

			String hl = "HL" + std::to_string(level);
			imshow(hl, (Mat_<uchar>) dest[i + 1]);
			resizeWindow(hl, height, width);

			String lh = "LH" + std::to_string(level);
			imshow(lh, (Mat_<uchar>) dest[i + 2]);
			resizeWindow(lh, height, width);

			level++;
		}

		imshow("LL", (Mat_<uchar>) dest[dest.size() - 1]);
		resizeWindow("LL", height, width);

		Mat_<int> dst = twoDConstructRecursive(dest);
		imshow("Img Construct", (Mat_<uchar>) dst);

		float mae = meanAbsoluteError(src, dst);
		printf("mae=%f", mae);

		Mat_<int> diff =scaling(src, dst);
		imshow("Difference", (Mat_<uchar>) diff);
		
		waitKey(0);
	}
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
		/*printf(" 4 - 2D Construction\n");
		printf(" 5 - 2D Deconstruction\n");*/
		printf(" 6 - 1D Construction\n");
		printf(" 7 - 1D Deconstruction\n");
		printf(" 8 - 2D Deconstruction\n");
		printf(" 9 - 2D Recursive Deconstruction\n");
		printf(" 10 - 2D Recursive Construction\n");
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
				/*char fname[MAX_PATH];
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
				}*/
				break;
			}
			case 5: {
				/*char fname[MAX_PATH];
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
				}*/
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
				twoDDeconstructShow();
				break;
			}
			case 9: {
				twoDDeconstructRecursiveShow();
				break;
			}
			case 10: {
				twoDConstructRecursiveShow();
				break;
			}
		
		}
	}
	while (op!=0);
	return 0;
}
