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
  /*src points to the memory address of the input data
    width and height are the width and height of the input data (in bytes)
    result points to the memory address of the encoded result
    
    THE FUNCTION RETURNS THE LENGHT OF THE OUTPUT IN BYTES
    
    RESTRICTIONS:
    - Contents of the buffer after the encoded output 
    should not be corrupted*/

  // Phase I
  if(width == 0 || height == 0) return 0;
  else{
    int length_counter = 0; // Counter of bits
    for(int i = 1 ; i <= height; i++){
      // Variables for each row
      u8 base = 0; // minimum FILTER
      u8 max_delta = 0;
      u8 number_bits; // 
      u8 row_start = 0;
      u8 row_end = width;
      for(int j = row_start ; j < i*row_end ; j++){
        //Variables for each number
        u8 pixel_back;
        u8 pixel_up;
        u8 pixel_back_up;
        u8 avg;
        u8 filter;

        // FIRST PART: LOCATING THE THREE BLOCKS AND CALCULATING AVERAGE
        // Back Assignment
        if(i % width == 0){
          pixel_back = 0;
        } else{ 
          pixel_back = src[i-1];
        }
        // Up assignment
        if(i < width){
          pixel_up = 0;
        }else{
          pixel_up = src[i-width];
        }
        // Up back assignment
        if(i < width || i % width == 0){
          pixel_back_up = 0;
        }else{
          pixel_back_up = src[i-width-1];
        }

        avg = (pixel_back + pixel_back_up + pixel_up) / 3;
        
        // SECOND PART: CALCULATING FILTER
        // FILTER IS THE PIXEL VALUE MINUS THE AVERAGE
        
        if(src[i] >= avg){
          filter = src[i] - avg;
        }else{
          filter = src[i] + 256 - avg;
        }
        // THIRD PART: CALCULATE THE BASE
        if(base > filter){
          base = filter;
        }
        row_start += width;
        row_end += width;
      }
      // PASS THE BASE TO THE RESULT ARRAY
      int shiftRight = length_counter % 8;
      u8 first_part = base >> shiftRight;
      u8 second_part;
      *(result + length_counter/8) = first_part | *(result + length_counter/8);

      if(shiftRight > 0){
        int shiftLeft = 8 - shiftRight;
        second_part = base << shiftLeft;
        length_counter += shiftLeft;
        *(result+length_counter/8) = second_part;
        length_counter += shiftRight;

      }else{
        length_counter += 8;
      }

      // LOOP FOR THE DELTAS
      // DELTA IS THE ORIGINAL VALUE MINUS THE BASE
      u8 row_start = 0;
      u8 row_end = width;
      for(int j = row_start ; j < i*row_end ; j++){
        u8 delta = src[i] - base;
        
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

      // PASS NUMBER OF BITS TO THE ARRAY
      // number of bits should be written in 4 bits
      u8 significant_bits = number_bits <<= 4;

      shiftRight = length_counter % 8;
      u8 first_part = significant_bits >> shiftRight;
      u8 second_part;
      *(result + length_counter/8) = first_part | *(result + length_counter/8);

      if(shiftRight > 4){

        int shiftLeft = 8 - shiftRight;
        length_counter += shiftLeft;
        second_part = significant_bits << shiftLeft;
        
        *(result+length_counter/8) = second_part;
        length_counter += 4 - shiftLeft;

      }else{
        length_counter += 4;
      }
      if(number_bits == 0){
        continue;
      }


      // PASS THE DELTAS IN THE RESULT ARRAY
      u8 row_start = 0;
      u8 row_end = width;
      for(int j = row_start ; j < i*row_end ; j++){
        u8 delta = src[i] - base;
        // PUSH HERE
          u8 significant_bits_delta = delta >>= (8-number_bits);

          shiftRight = length_counter % 8;
          u8 first_part = significant_bits_delta >> shiftRight;
          u8 second_part;
          *(result + length_counter/8) = first_part | *(result + length_counter/8);

          if(shiftRight > 8 - number_bits){
            int shiftLeft = 8 - shiftRight;
            length_counter += shiftLeft;
            second_part = significant_bits_delta << shiftLeft;
            
            *(result+length_counter/8) = second_part;
            length_counter += number_bits - shiftLeft;

          }else{
            length_counter += 8;
        }
      }
    }
  }
  return 0;
}

