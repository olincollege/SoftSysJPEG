# JPEG Compression in C
Krishna Suresh and Merwan Yeditha

## Description
**Our Goal**

With this project, we wanted to implement a program in C that could take a bitmap image as an impit and write a JPEG file (more specifically JFIF)  as an output that would utilize typical compression techniques to reduce the size of the file. This specifically entails subsampling the image, converting it from the RGB colorspace to YCbCr, utilizing the Discrete Cosine transform to express blocks of the image as sums of cosines, and encode the data using huffman encoding. A properly working program should produce a JPEG file that looks similar to the original image while being smaller in file size.

**Learning Goals**

With this project, we were really hoping to increase our understanding of C, particularly using structs and malloc operations. Additionally, we wanted to gain experience with Huffman encoding, as it involved the implementation of binary trees and priority queues, which are data structures we were familair with before the project but had not implemented in C before. Finally, we wanted to gain experience using existing libraries in C by going through their documentation.

## State of the Project
We have been able to implement all of the compression and storage techniques that are typically involved in the creation of JPEGs and have an output of the same dimensions as the input, but the output currently does not resemble the original image. 

### Compression Steps
**YCbCr Conversion**

The first step with JPEG compression is to convert RBG-colorspace images to YCbCr. The Y component is a grayscale copy of the original image, while Cb and Cr are two Chroma components. Chrome componetns are not perceived as much as grayscale, so separating the image into these components is helpful as we can downsample and compress the chroma components.

**Chroma Downsampling**

We utilized what is known as a 4:2:2 downsample. This downsampling technique takes a 4x2 block of pixels and organizes them into four 2x1 blocks (image shown below). By downsampling the chroma we end up with less total colors to represent.

![Chroma Subsampling](imgs/subsample.png)

To simplify this, we just averaged each 2x1 block. Below is the code we ran on each 2x1 block inside of a nested for-loop.

```C
cb_avg = (*((cb+row*width)+col)+ *((cb+row+1*width)+col))/2;
*((cb+row*width)+col) = cb_avg;
*((cb+row+1*width)+col) = cb_avg;
cr_avg = (*((cr+row*width)+col)+ *((cr+row+1*width)+col))/2;
*((cr+row*width)+col) = cr_avg;
*((cr+row+1*width)+col) = cr_avg;
```

**Discrete Cosine Transform and Quantization**

The Discrete Cosine Transform aims to express images as a weighted sum of sinusoids. When applied in two dimensions, this can be used to represent images as the weighted sum of sinusoids acting in both the x and y direction. Below is a sample of what the sinusoid components would look like.

![Discrete Cosine Transform Visualized](imgs/dct.png)

This transform is applied to an 8x8 block in an image. Components further to the bottom-right tend to be less consequential to the image that a human can see and images tend to have smaller components of these sinusoids, so we can reduce the weight of these components. This is done through the use of quantization. We divide every number by a "quantum", stored in an 8x8 quantization table. This table is designed to reduce the weight of values further to the bottom-right. 

We implenented the DCT and quantization using GSL (GNU Scientific Library). We chose to do this becasue we wanted experience using existing libraries and felt that implementing matrix operations ourselves would not align with our learning goals and the end product would likely be slower than the GSL implementations. The GSL math is shown below.

```c
    gsl_matrix *inter = gsl_matrix_alloc(MAT_SIZE, MAT_SIZE);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, t, m, 0.0, inter);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inter, t, 0.0, res)
```

**Huffman Encoding**

JPEGs then encode this transformed data using Huffman codes, which is a form of lossless encoding. Huffman encoding places values in a tree where values that show up more frequently appear closer to the head node of the tree and values that occur less frequently appear further down the tree. Values are then represented as the path it takes to get to them in binary. A '0' represents a left branch, and a '1' represents a right branch.

![Huffman Tree](imgs/huff_tree.png)

In this case, the encoding for D would be '11', which is only two bits. JPEGs use this to create Huffman tables. We implemented huffman encoding but were unable to create functionality to write to a JPEG, so we used an open-source huffman encoder for JPEGs.

## Retrospective

Our final product was definitely past our MVP but did not reach our full goal. We wanted to implement every part of the JPEG compression from top to bottom, and while we were able to implement huffman encoding on its own, we were not able to fully integrate it with our JPEG writing code in time. Therefore, we adapted code we found online to encode data and write it to the JPEG. However, we still coded the DCT, unpacked the bitmap, and understood huffman coding and writing JPEG files well enough that we were able to refactor existing code to work for us. We feel like given the effort we put in and what we were able to do, we definitely reached our learning goals for this project.
