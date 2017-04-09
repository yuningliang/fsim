// binary search of an integer array, this search is efficient for large arrays
// tested with PellesC       vegaseat     24jan2005
 
#include <stdio.h>
 
int binarySearch(int *array, int number, int target)
{
  int *beg, *end, *mid;
  int n = number;

  beg = &array[0];
  end = &array[n-1];
  mid = beg + n/2;
  // binary search, there is an AND in the middle of while()!!!
  while((beg <= end) && (*mid != target))
  {
    // is the target in lower or upper half?
    if (target < *mid)
    {
      end = mid - 1;     // new end
    }
    else
    {
      beg = mid + 1;     // new beginning
    }
    mid = beg + (end - beg) / 2;
  }
  
  // did you find the target?
  if (*mid == target)
    return mid-array;
  else
    return -1;
}

int main()
{
  int a[15] = {999, 888, 776, 23245, 1, -1, -4, 800000, 90, 0xfffffffd, 0xfffc, 789, 123456, 654, 99}; 
  int n, i, j, temp;
  int *beg, *end, *mid, target;
  
  n = 15;
  if (n >= 20) return 0;   // ouch!
    
  // sort the loaded array, a must for binary search! 
  // you can apply qsort or other algorithms here
  for(i = 0; i < n-1; i++)
  {  
    for(j = 0; j < n-i-1; j++)
    {
      if (a[j+1] < a[j])
      {
        temp = a[j];
        a[j] = a[j+1];
        a[j+1] = temp;
      }
    }
  }
    
  int cc = 0;
  for (i=0; i<n; i++) {
    if (binarySearch(a, 15, a[i]) < 0) {
      printf("%d not found\n", a[i]);
      cc ++;
    }
  }
  

  for (i=65536; i<70000; i++) {
    if (binarySearch(a, 15, i) >= 0) {
      printf("Incorrectly search:%d\n", i);
      cc ++;
    }
  }
  printf("passes of correctly searched:%d\n", cc);
  return 0;
}
