//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2022)
//
//  Project #1:
//
//  September 6, 2022
//
//  Seongyeop Jeong (seongyeop.jeong@snu.ac.kr)
//  Jaehoon Shim (mattjs@snu.ac.kr)
//  IlKueon Kang (kangilkueon@snu.ac.kr)
//  Wookje Han (gksdnrwp@snu.ac.kr)
//  Jinsol Park (jinsolpark@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------

typedef unsigned char u8;

/* TODO: Implement this function */
int encode(const u8* src, int width, int height, u8* result)
{
  /*INPUTS
    src points to the memory address of the input data
    width and height are the width and height of the input data (in bytes)
    result points to the memory address of the encoded result

    OUTPUT
    THE FUNCTION RETURNS THE LENGHT OF THE OUTPUT IN BYTES
    
    RESTRICTIONS:
    - Contents of the buffer after the encoded output 
    should not be corrupted*/

  int length_counter = 0; // Counter of bits that are already written
  if(width == 0 || height == 0) return 0; 
  else{
    // locate the start of a row
    u8 row_start = 0;
    u8 row_end = width;
    
    for(int i = 0; i < height; i++){
      // Variables for each row
      u8 base = 255; // minimum FILTER, initialized in 255 as it is the max possible value
      u8 max_delta = 0; //maximum DELTA, initialized in 0 as it is the min possible value
      u8 number_bits; // number of bits in which the deltas are encoded

      // inner loop for each element in a row
      for(int j = row_start ; j < row_end ; j++){
        //Variables for each element
        u8 pixel_back = 0;
        u8 pixel_up = 0;
        u8 pixel_back_up = 0; // pixels for finding the average
        u8 avg; // average
        u8 filter;

        // FIRST PART: LOCATING THE THREE BLOCKS AND CALCULATING AVERAGE
        // Back Assignment
        u8 divider = 3;
        if(j % width == 0){
          divider--;
        } else{ 
          pixel_back = *(src + j-1);
        }
        // Up assignment
        if(j < width){
          divider--;
        }else{
          pixel_up = *(src+j-width);
        }
        // Up back assignment
        if(j < width || j % width == 0){
          divider--;
        }else{
          pixel_back_up = *(src+j-width-1);
        }
        if(divider == 0) divider = 1;
        avg = (pixel_back + pixel_back_up + pixel_up) / divider;

        // SECOND PART: CALCULATING FILTER
        if(*(src+j) >= avg){
          // FILTER IS THE PIXEL VALUE MINUS THE AVERAGE
          filter = *(src+j)  - avg;
        }else{
          filter = *(src+j)  + 256 - avg;
        }
        // THIRD PART: CALCULATE THE BASE
        if(base > filter) {
          base = filter;
        }

      }

      // ENCODING THE BASE TO THE RESULT ARRAY

      // Knowing how many bits of the lastest chunk are being used already
      int shift_right = length_counter % 8;
      u8 first_part_base = base >> shift_right;

      // Ensuring that the new chunk space is filled with 0
      *(result + length_counter/8) = 255 << (8-shift_right) & *(result + length_counter/8);
      // Putting the first part ahead of the written part
      *(result + length_counter/8) = first_part_base | *(result + length_counter/8);

      if(0 < shift_right){
        int shiftLeft = 8 - shift_right;
        u8 second_part_base = base << shiftLeft;
        length_counter += shiftLeft;
        *(result+length_counter/8) = second_part_base | 0;

        // Settling to 0 the rest of the chunk
        length_counter += shift_right;
        if(length_counter % 8 != 0) {
        u8 shiftLeft = 8 - (length_counter % 8);
        u8 zeros = 0;
        u8 zeros_bits = zeros << shiftLeft;
        *(result + length_counter/8) = (*(result + length_counter/8) & zeros_bits) | *(result + length_counter/8);
        
        }
      }else{
        length_counter += 8;
      }

      // LOOP FOR THE DELTAS
      
      for(int j = row_start ; j < row_end ; j++){

        u8 pixel_back = 0;
        u8 pixel_up = 0;
        u8 pixel_back_up = 0;
        u8 avg;
        u8 filter;

        u8 divider = 3;
        if(j % width == 0){
          divider--;
        } else{ 
          pixel_back = *(src + j-1);
        }
        if(j < width){
          divider--;
        }else{
          pixel_up = *(src+j-width);
        }
        if(j < width || j % width == 0){
          divider--;
        }else{
          pixel_back_up = *(src+j-width-1);
        }
        if(divider == 0) divider = 1;
        avg = (pixel_back + pixel_back_up + pixel_up) / divider;
        if(*(src+j) >= avg){
          filter = *(src+j)  - avg;
        }else{
          filter = *(src+j)  + 256 - avg;
        }
        // DELTA IS THE ORIGINAL VALUE MINUS THE BASE
        u8 delta = filter - base;

        // COMPARE TO THE MAX DELTA
        if(max_delta < delta){
          max_delta = delta;
        }
      }

      //FIND THE NUMBER OF BITS (n(i))
      if(max_delta == 0) number_bits = 0;
      else if(max_delta == 1) number_bits = 1;
      else if(max_delta < 4) number_bits = 2;
      else if(max_delta < 8) number_bits = 3;
      else if(max_delta < 16) number_bits = 4;
      else if(max_delta < 32) number_bits = 5;
      else if(max_delta < 64) number_bits = 6;
      else if(max_delta < 128) number_bits = 7;
      else number_bits = 8;

     // ENCODE NUMBER OF BITS TO THE RESULT
     // The number of bits should be written in 4 bits
      u8 significant_bits = number_bits << 4;

      u8 shiftRight = length_counter % 8;
      u8 first_part = significant_bits >> shiftRight;
      *(result + length_counter/8) = 255 << (8-shiftRight) & *(result + length_counter/8);
      *(result + length_counter/8) = first_part | *(result + length_counter/8);

      if(shiftRight > 8 - 4){
        int shiftLeft = 8 - shiftRight;
        length_counter += shiftLeft;
        u8 second_part = significant_bits << shiftLeft;
        
        *(result+length_counter/8) = second_part | 0;
        
        length_counter += 4 - shiftLeft;
        if(length_counter % 8 != 0) {
        u8 shiftLeft = 8 - (length_counter % 8);
        u8 zeros = 0;
        u8 zeros_bits = zeros << shiftLeft;
        *(result + length_counter/8) = (*(result + length_counter/8) & zeros_bits) | *(result + length_counter/8);
        
        }
      }
      
      else{
        length_counter += 4;
      }

      // ENCODE THE DELTAS IN THE RESULT ARRAY
      
      for(int j = row_start ; j < row_end ; j++){

        // YOU SHOULDNT WRITE ANY DELTAS WHEN THE NUMBER OF BITS IS 0
        if(number_bits == 0) continue;
        else{
          //Variables for each number
          u8 pixel_back = 0;
          u8 pixel_up = 0;
          u8 pixel_back_up = 0;
          u8 avg;
          u8 filter;
          u8 divider = 3;
          if(j % width == 0){
            divider--;
          } else{ 
            pixel_back = *(src + j-1);
          }
          if(j < width){
            divider--;
          }else{
            pixel_up = *(src+j-width);
          }
          if(j < width || j % width == 0){
            divider--;
          }else{
            pixel_back_up = *(src+j-width-1);
          }
          if(divider == 0) divider = 1;
          avg = (pixel_back + pixel_back_up + pixel_up) / divider;
          if(*(src+j) >= avg){
            filter = *(src+j)  - avg;
          }else{
            filter = *(src+j)  + 256 - avg;
          }

          u8 delta = filter - base;

          //PUSHING PART
          // Deltas should be encoded in a number_bits size
          u8 significant_bits_delta = delta << (8 - number_bits);

          shiftRight = length_counter % 8;
          u8 first_part = significant_bits_delta >> shiftRight;
          u8 second_part;
          *(result + length_counter/8) = 255 << (8-shiftRight) & *(result + length_counter/8);
          *(result + length_counter/8) = first_part | *(result + length_counter/8);
          if(shiftRight > 8 - number_bits){
            int shiftLeft = 8 - shiftRight;
            length_counter += shiftLeft;
            second_part = significant_bits_delta << shiftLeft;
            *(result+length_counter/8) = second_part | 0;
            length_counter += number_bits- shiftLeft;
            if(length_counter % 8 != 0) {
              u8 shiftLeft = 8 - (length_counter % 8);

              u8 zeros = 0;
              u8 zeros_bits = zeros << shiftLeft;
              *(result + length_counter/8) = (*(result + length_counter/8) & zeros_bits) | *(result + length_counter/8);
              
            }
            }else{
                length_counter += number_bits;
          }
        }
          
      }
      // NEW ROW ASSIGNMENT
      row_start += width;
      row_end += width;
    }
    //PADDING CODE, similar to the one used for each "second_part" for any pushable data
    if(length_counter % 8 != 0) {
      u8 shiftLeft = 8 - (length_counter % 8);
      u8 zeros = 0;
      u8 zeros_bits = zeros << shiftLeft;
      *(result + length_counter/8) = *(result + length_counter/8) | zeros_bits;
      length_counter += shiftLeft;
    }
  }
  return length_counter/8;
}

