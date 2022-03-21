#include "seam_carver.hpp"
#include <iostream>

// implement the rest of SeamCarver's functions here

const ImagePPM& SeamCarver::GetImage() const{
  return image_;
}

int SeamCarver::GetHeight() const{
  return height_;
}

int SeamCarver::GetWidth() const{
  return width_;
}

int SeamCarver::GetEnergy(int row, int col) const{
  int prevcol = col-1;
  int prevrow = row-1;
  int nextcol = col+1;
  int nextrow = row+1;
  if(col == 0){
    prevcol = width_-1;
  }
  if (col == width_-1) {
    nextcol = 0;
  }
  if(row == 0) {
    prevrow = height_-1;
  }
  if (row == height_-1) {
    nextrow = 0;
  }
  int rowr = image_.GetPixel(row, prevcol).GetRed() - image_.GetPixel(row, nextcol).GetRed();
  int rowg = image_.GetPixel(row, prevcol).GetGreen() - image_.GetPixel(row, nextcol).GetGreen();
  int rowb = image_.GetPixel(row, prevcol).GetBlue() - image_.GetPixel(row, nextcol).GetBlue();
  int colr = image_.GetPixel(prevrow, col).GetRed() - image_.GetPixel(nextrow, col).GetRed();
  int colg = image_.GetPixel(prevrow, col).GetGreen() - image_.GetPixel(nextrow, col).GetGreen();
  int colb = image_.GetPixel(prevrow, col).GetBlue() - image_.GetPixel(nextrow, col).GetBlue();
  return rowr*rowr+rowg*rowg+rowb*rowb+colr*colr+colg*colg+colb*colb;
}

int* SeamCarver::GetHorizontalSeam() const{
  //declare energy matrix
  int** energy_mtx = new int*[height_];
  for(int i = 0; i < height_ ; i++) {
    energy_mtx[i] = new int[width_];
  }

  //Declare last row
  for(int r = 0; r < height_; r++) {
    energy_mtx[r][width_-1] = GetEnergy(r, width_-1);
  }

  //finish the dynamic matrix
  for(int c = width_-2; c > -1; c--) {
    for(int r = 0; r < height_; r++) {
      energy_mtx[r][c] = energy_mtx[r][c+1];
      if(r < height_-1 && energy_mtx[r+1][c+1] < energy_mtx[r][c]) {
        energy_mtx[r][c] = energy_mtx[r+1][c+1];
      }
      if(r > 0 && energy_mtx[r-1][c+1] < energy_mtx[r][c]) {
        energy_mtx[r][c] = energy_mtx[r-1][c+1];
      }

      energy_mtx[r][c] += GetEnergy(r, c);
    }
  }

  //find index of smallest value in last row
  int i = 0;
  for(int r = 1; r < height_; r++) {
    if(energy_mtx[i][0] > energy_mtx[r][0]) {
      i = r;
    }
  }
  //initialize returning row
  int* row = new int[width_];
  row[0] = i;

  //fill returning row
  for(int c = 1; c < width_; c++) {
    row[c] = i;
    if(i < height_-1 && energy_mtx[row[c]][c] > energy_mtx[i+1][c]) {
      row[c] = i+1;
    }
    if(i > 0 && energy_mtx[row[c]][c] >= energy_mtx[i-1][c]) {
      row[c] = i-1;
    }
    i = row[c];
  }

  //get rid of the matrix
  for(int i = 0; i < height_ ; i++) {
    delete[] energy_mtx[i];
  }
  delete[] energy_mtx;
  energy_mtx = nullptr;
  return row;
}

int* SeamCarver::GetVerticalSeam() const{
  //declare energy matrix
  int** energy_mtx = new int*[height_];
  for(int i = 0; i < height_ ; i++) {
    energy_mtx[i] = new int[width_];
  }

  //Declare last col
  for(int c = 0; c < width_; c++) {
    energy_mtx[height_-1][c] = GetEnergy(height_-1, c);
  }

  //finish the dynamix matrix
  for(int r = height_-2; r > -1; r--) {
    for(int c = 0; c < width_; c++) {
      energy_mtx[r][c] = energy_mtx[r+1][c];
      if(c > 0 && energy_mtx[r+1][c-1] < energy_mtx[r][c]) {
        energy_mtx[r][c] = energy_mtx[r+1][c-1];
      }
      if(c < width_-1 && energy_mtx[r+1][c+1] < energy_mtx[r][c]) {
        energy_mtx[r][c] = energy_mtx[r+1][c+1];
      }
      energy_mtx[r][c] += GetEnergy(r, c);
    }
  }

  //find index of smallest value in last row
  int i = 0;
  for(int c = 1; c < width_; c++) {
    if(energy_mtx[0][i] > energy_mtx[0][c]) {
      i = c;
    }
  }

  //initialize returning row
  int* col = new int[height_];
  col[0] = i;

  //fill returning row
  for(int r = 1; r < height_; r++) {
    col[r] = i;
    if(i < width_-1 && energy_mtx[r][col[r]] > energy_mtx[r][i+1]) {
      col[r] = i+1;
    }
    if(i > 0 && energy_mtx[r][col[r]] >= energy_mtx[r][i-1]) {
      col[r] = i-1;
    }


    i = col[r];
  }

  //get rid of the matrix
  for(int i = 0; i < height_ ; i++) {
    delete[] energy_mtx[i];
  }
  delete[] energy_mtx;
  energy_mtx = nullptr;
  return col;
}

void SeamCarver::RemoveHorizontalSeam(){

  int* tocarve = GetHorizontalSeam();

  auto** newimage = new Pixel*[height_-1];
  for(int i = 0; i < height_-1 ; i++) {
    newimage[i] = new Pixel[width_];
  }
  for(int c = 0; c < width_; c++) {
    for(int r = 0; r < height_-1; r++) {
      if(r < tocarve[c]) {
        newimage[r][c] = image_.GetPixel(r,c);
      } else {
        newimage[r][c] = image_.GetPixel(r+1, c);
      }
    }
  }

  image_.SetImagePPM(newimage, height_-1, width_, image_.GetMaxColorValue());
  height_ --;
  delete[] tocarve;
  tocarve = nullptr;
}

void SeamCarver::RemoveVerticalSeam(){
  int* tocarve = GetVerticalSeam();

  auto** newimage = new Pixel*[height_];
  for(int i = 0; i < height_; i++) {
    newimage[i] = new Pixel[width_-1];
  }

  for(int r = 0; r < height_; r++) {
    for(int c = 0; c < width_-1; c++) {
      if(c < tocarve[r]) {
        newimage[r][c] = image_.GetPixel(r,c);
      } else {
        newimage[r][c] = image_.GetPixel(r, c+1);
      }
    }
  }
  image_.SetImagePPM(newimage, height_, width_-1, image_.GetMaxColorValue());
  width_ --;
  delete[] tocarve;
  tocarve = nullptr;
}

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}
