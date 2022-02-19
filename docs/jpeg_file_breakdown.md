# JPEG File Structure

## File Structure
![](/docs/jpegstructure.jpg)
### Start of Image
`FF D8`
Image marker thing
### Header stuff
`FF E0`
Header info stuff
### Start of Frame
`FF C0`
Data about the color encoding?
### Start of Frame2
`FF C2`
Not sure why theres two
### Define Huffman Tables 
`FF C4`
Not sure exactly why there are so many of these 
### Define Quantization Table
`FF DB`
This is the DCT thing but idk why there are two

### Start of Scan
`FFDA`
Header before the image data stuff.
### Image Data
Not sure how this is encoded yet
### Restart
Some stuff related to restart and restart interval that didn't make too much sense yet.


## Compression Steps
> Taken from https://docs.fileformat.com/image/jpeg/
1. Transformation: RGB -> YCrCb format
2. Down Sampling: Color components are halved to compress further and not impact image that much
3. Groups: 
4. DCT: This is the discrete cos transform applied to the groups
5. Quantization: Don't understand this too well yet
6. Encoding: Huffman coding stuff
7. Headers

Do the opposite to decode