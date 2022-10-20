
# LSB Stegonography

The project hides a secret message within a bigger one so that neither its existence nor its content can be discovered.

## Screenshots

![Screenshot from 2022-08-31 23-32-2](https://user-images.githubusercontent.com/56476333/187748513-66c7f6c9-f0a6-4137-b6b0-6a36159501b8.png)

## Installation

Compilation with gcc

```bash
git clone https://github.com/vignesh4a/Stegonography.git
  
cd Steganography

gcc *.c -o stego.out 

```
    
## Usage/Examples

#### For Encoding :
```shell
./stego.out -e Base_Image.bmp Secret.txt [stego.bmp]
```

#### For decoding :
```shell
./stego.out -d stego.bmp [decode.txt]
```

