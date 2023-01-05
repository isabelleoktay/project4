/* Author: Isabelle Oktay
 * Version: 05.3.2021
 * Description: My program reads information about an existing Translation Lookaside Buffer (TLB),
 *              the Virtual Address Page table, and Physical Memory Direct Memory caching.
 *              Based on storing this information, my program prompts the user a virtual address (in hex)
 *              and outputs the byte at that location.
 *              My program prints out "Can not be determined" if the data is not available.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* A node structure for use with the
 * TLB. Each value in the TLB contains:
 *      - an index
 *      - a tag
 *      - a physical page number
 */
typedef struct TLB
{
    int index;
    int tag;
    int PPN;
    struct TLB* nextTLB;
} TLB;

/* A node structure for use with the
 * Page Table. Each page contains:
 *      - a virtual page number
 *      - a physical page number
 */
typedef struct Page
{
    int VPN;
    int PPN;
    struct Page *nextPage;
} Page;

/* A node structure for use with the
 * Cache. Each value in the Cache contains:
 *      - a cache index
 *      - a cache tag
 *      - 4 offsets
 */
typedef struct Cache
{
    int cacheIndex;
    int cacheTag;
    int offset0;
    int offset1;
    int offset2;
    int offset3;
    struct Cache *nextCache;

} Cache;

/* Adds a value to the TLB.
 * Parameters:
 *  head - the pointer to the head of the TLB
 *  index - the index value to add
 *  tag - the tag value to add
 *  PPN - the physical page number to add
 * Return value:
 *  none
 */
void addTLB(TLB** head, int index, int tag, int PPN);

/* Adds a value to the Page Table.
 * Parameters:
 *  head - the pointer to the head of the Page Table
 *  VPN - the virtual page number to add
 *  PPN - the physical page number to add
 * Return value:
 *  none
 */
void addPage(Page** head, int VPN, int PPN);

/* Adds a value to the Cache.
 * Parameters:
 *  head - the pointer to the head of the Cache
 *  cacheIndex - the cache index value to add
 *  cacheTag - the cache tag value to add
 *  o0 - the first offset value to add
 *  o1 - the second offset value to add
 *  o2 - the third offset value to add
 *  o3 - the fourth offset value to add
 * Return value:
 *  none
 */
void addCache(Cache** head, int cacheIndex, int cacheTag, int o0, int o1, int o2, int o3);

/* Converts a hex address to its decimal equivalent and stores each value in an array.
 * Also determines if the input is valid or not
 * Parameters:
 *  hex - the hex address to convert
 * Return value:
 *  true if address is valid, false otherwise
 */
bool hexadecimalToInteger(char *hex);

/* Converts a decimal value to its hex equivalent
 * Parameters:
 *  dec - the decimal value to convert
 * Return value:
 *  none
 *  - BUT it will print out each of the individual in the total hex value
 */
void decToHex(int dec);

// array to store converted hex address in
int hexToIntConverted[4];
// the array to store the converted dec to hex final bit
int intToHex[4];

int main() {
    // input data prep
    char data[60];
    char *ptr;

    // references to the heads of the (now empty) TLB, Page Table, and Cache
    struct TLB *tlbHead = NULL;
    struct Page *pageHead = NULL;
    struct Cache *cacheHead = NULL;
    long PPN;

    // direct the standard input stream to the file for Project 4
    FILE *in = fopen("Project4Input.txt", "r");

    // loop through input file until there are no more lines
    while(fgets(data, 800, in) != NULL) {
       const char delimiter[1] = ",";

       ptr = strtok(data, delimiter);
       int n = 0;
       char *tempArray[8];
       char *endPtr;

       // we want to separate each line based on commas and store each separate item in the temp array
       while (ptr != NULL) {
           tempArray[n++] = ptr;
           ptr = strtok(NULL, delimiter); // this will clear the current value and move on to the next one
       }

       // in order to store the values in our linked list, we need to check if the first value says "TLB"
       if (strcmp(tempArray[0], "TLB") == 0) {
           // convert the strings to their int equivalents
           long tlbIndex = strtol(tempArray[1], &endPtr, 16);
           long tlbTag = strtol(tempArray[2], &endPtr, 16);
           PPN = strtol(tempArray[3], &endPtr, 16);

           // add the values to the TLB
           addTLB(&tlbHead, (int) tlbIndex, (int) tlbTag, (int) PPN);
       }
       else if (strcmp(tempArray[0], "Page") == 0) {
           // convert the strings to their int equivalents
           long VPN = strtol(tempArray[1], &endPtr, 16);
           PPN = strtol(tempArray[2], &endPtr, 16);

           // add the values to the Page Table
           addPage(&pageHead, (int) VPN, (int) PPN);
       }
       else if (strcmp(tempArray[0], "Cache") == 0) {
           // convert the strings to their int equivalents
           long cacheIndex = strtol(tempArray[1], &endPtr, 16);
           long cacheTag = strtol(tempArray[2], &endPtr, 16);
           long o0 = strtol(tempArray[3], &endPtr, 16);
           long o1 = strtol(tempArray[4], &endPtr, 16);
           long o2 = strtol(tempArray[5], &endPtr, 16);
           long o3 = strtol(tempArray[6], &endPtr, 16);

           // add the values to the Cache
           addCache(&cacheHead, (int) cacheIndex, (int) cacheTag, (int) o0, (int) o1, (int) o2, (int) o3);
       }
       // if we have reached 'DONE', this means that the file input has stopped and its time to read in
       // user inputs
       else if (strcmp(tempArray[0], "DONE") == 0) {
           // user input prep
           int i;
           char *address = (char*) malloc(256*sizeof(char));

           // we are only taking in 3 user inputs
           for (i = 0; i < 3; i++) {
               bool valid = true; // keeping track of if the address is valid

               // get user input
               printf("Enter Virtual Address (in HEX): 0x");
               scanf(" %s", address);

               // check if input is a valid length
               if (strlen(address) > 4) {
                   valid = false;
                   printf("Cannot be determined. \n");
                   continue;
               }

               // check if all values in input are valid
               // also convert to int equivalent
               valid = hexadecimalToInteger(address);
               if (valid == false) {
                   printf("Cannot be determined. \n");
                   continue;
               }

               // convert input into the binary address!!
               PPN = -1;
               int data0 = 0;
               int data1 = 0;
               int data2 = 0;
               int data3 = 0;
               int binAddress = 0;

               // we need to shift them over so that they will fit the 14 bit address
               data0 = hexToIntConverted[0];
               data0 <<= 12;
               data1 = hexToIntConverted[1];
               data1 <<= 8;
               data2 = hexToIntConverted[2];
               data2 <<= 4;
               data3 = hexToIntConverted[3];
               binAddress = data0 | data1 | data2 | data3;

               // get virtual page offset
               int VPO = binAddress & 0b00000000111111;
               // get tlb index
               int TLBI = (binAddress & 0b00000011000000);
               TLBI >>= 6;
               // get tlb tag
               int TLBT = binAddress & 0b11111100000000;
               TLBT >>= 8;
               // get virtual page number
               int VPN = binAddress & 0b11111111000000;
               VPN >>= 6;

               // see if tlb contains index and tag that matches with the values of the address
               struct TLB *tlbCurrent = tlbHead;
               while (tlbCurrent != NULL) {
                   // iterating through all of the nodes in the tlb linked list
                   if (tlbCurrent->index == TLBI && tlbCurrent->tag == TLBT) {
                       PPN = tlbCurrent->PPN;
                       break;
                   }
                   tlbCurrent = tlbCurrent->nextTLB;
               }

               // if the tlb does not contain an index and tag that matches, then we need to
               // check the page table and see if there is a virtual address that matches
               if (tlbCurrent == NULL) {
                   struct Page *pageCurrent = pageHead;
                   while (pageCurrent != NULL) {
                       // iterating through all of the nodes in the Page Table linked list
                       if (pageCurrent->VPN == VPN) {
                           PPN = pageCurrent->PPN;
                           break;
                       }
                       pageCurrent = pageCurrent->nextPage;
                   }
               }

               // at this point, if the PPN has not been found,
               // then, we can assume that the value cannot be determined and move onto the next input
               if (PPN == -1) {
                   printf("Can not be determined. \n");
                   continue;
               }

               // if the physical page number has been found
               // then we convert the virtual page offset and physical page number into
               // their cache value offset, index, and tag
               int CO = VPO & 0b000011;
               int CI = VPO & 0b111100;
               CI >>= 2;
               int CT = PPN;

               // valid will be used to check if the offset has been correct or not
               // as we loop through the cache. i.e., if the value has been found, then in the end
               // we don't need to print "Cannot Be determined" (which is another conditional)
               valid = false;
               struct Cache *cacheCurrent = cacheHead;
               while (cacheCurrent != NULL) {
                   // see if the index and tag we have found matches any of the stored combinations
                   if (cacheCurrent->cacheIndex == CI && cacheCurrent->cacheTag == CT) {
                       if (CO == 0b00) {
                           printf("0x");
                           decToHex(cacheCurrent->offset0);
                           valid = true; // this valid will be turned true if there is a match with the offset too
                       }
                       else if (CO == 0b01) {
                           printf("0x");
                           decToHex(cacheCurrent->offset1);
                           valid = true;
                       }
                       else if (CO == 0b10) {
                           printf("0x");
                           decToHex(cacheCurrent->offset2);
                           valid = true;
                       }
                       else if (CO == 0b11) {
                           printf("0x");
                           decToHex(cacheCurrent->offset3);
                           valid = true;
                       }
                       break;
                   }
                   cacheCurrent = cacheCurrent->nextCache;
               }

               // HOWEVER, if there is no match, then we need to indicate that the value cannot be determined
               // to the user. This is triggered if valid is false
               if (valid == false) {
                   printf("Cannot be determined. \n");
               }
           } // for loop for 3 user inputs

           // free the address
           free(address);
       }
    }

    // close the input file
    fclose(in);

    // free the TLB linked list
    TLB *freeTLBCurrent = tlbHead;
    while (freeTLBCurrent != NULL) {
        free(freeTLBCurrent);
        freeTLBCurrent = freeTLBCurrent->nextTLB;
    }

    // free the Page Table linked list
    Page *freePageCurrent = pageHead;
    while (freePageCurrent != NULL) {
        free(freePageCurrent);
        freePageCurrent = freePageCurrent->nextPage;
    }

    // free the Cache linked list
    struct Cache *freeCacheCurrent = cacheHead;
    while (freeCacheCurrent != NULL) {
        free(freeCacheCurrent);
        freeCacheCurrent = freeCacheCurrent->nextCache;
    }

    return 0;
}

/* Adds a value to the TLB.
 * Parameters:
 *  head - the pointer to the head of the TLB
 *  index - the index value to add
 *  tag - the tag value to add
 *  PPN - the physical page number to add
 * Return value:
 *  none
 */
void addTLB(TLB** head, int index, int tag, int PPN) {
    // allocate memory to a new node
    struct TLB *newTLB = (struct TLB*) malloc(sizeof(struct TLB));

    // assign values to structure
    newTLB->index = index;
    newTLB->tag = tag;
    newTLB->PPN = PPN;
    newTLB->nextTLB = NULL;

    // check if the list is empty and made the head the first node
    if (*head == NULL) {
        *head = newTLB;
    }
    // if the list is not empty, we just add to each node after that
    else {
        struct TLB *current = *head;
        // iterate until we get to the last node
        while (current->nextTLB != NULL) {
            current = current->nextTLB;
        }
        current->nextTLB = newTLB; // assign new node at the end
    }
}

/* Adds a value to the Page Table.
 * Parameters:
 *  head - the pointer to the head of the Page Table
 *  VPN - the virtual page number to add
 *  PPN - the physical page number to add
 * Return value:
 *  none
 */
void addPage(Page** head, int VPN, int PPN) {
    // allocate memory to a new node
    struct Page *newPage = (struct Page*) malloc(sizeof(struct Page));

    // assign values to structure
    newPage->VPN = VPN;
    newPage->PPN = PPN;
    newPage->nextPage = NULL;

    // check if the list is empty and made the head the first node
    if (*head == NULL) {
        *head = newPage;
    }
    // if the list is not empty, we just add to each node after that
    else {
        struct Page *current = *head;
        // iterate until we get to the last node
        while (current->nextPage != NULL) {
            current = current->nextPage;
        }
        current->nextPage = newPage; // assign new node at the end
    }
}

/* Adds a value to the Cache.
 * Parameters:
 *  head - the pointer to the head of the Cache
 *  cacheIndex - the cache index value to add
 *  cacheTag - the cache tag value to add
 *  o0 - the first offset value to add
 *  o1 - the second offset value to add
 *  o2 - the third offset value to add
 *  o3 - the fourth offset value to add
 * Return value:
 *  none
 */
void addCache(Cache** head, int cacheIndex, int cacheTag, int o0, int o1, int o2, int o3) {
    // allocate memory to a new node
    struct Cache *newCache = (struct Cache*) malloc(sizeof(struct Cache));

    // assign values to structure
    newCache->cacheIndex = cacheIndex;
    newCache->cacheTag = cacheTag;
    newCache->offset0 = o0;
    newCache->offset1 = o1;
    newCache->offset2 = o2;
    newCache->offset3 = o3;
    newCache->nextCache = NULL;

    // check if the list is empty and made the head the first node
    if (*head == NULL) {
        *head = newCache;
    }
    // if the list is not empty, we just add to each node after that
    else {
        struct Cache *current = *head;
        // iterate until we get to the last node
        while (current->nextCache != NULL) {
            current = current->nextCache;
        }
        current->nextCache = newCache; // assign new node at the end
    }
}

/* Converts a hex address to its decimal equivalent and stores each value in an array.
 * Also determines if the input is valid or not
 * Parameters:
 *  hex - the hex address to convert
 * Return value:
 *  true if address is valid, false otherwise
 */
bool hexadecimalToInteger(char *hex) {
    bool valid = true; // our bool value to eventually return
    int i;
    int length = strlen(hex);

    // check what the length is. Since addresses can be 3-4 long, we need to control for this
    // if the address is only 3 long, we add a 0 at the front
    if (length == 3) {
        hexToIntConverted[0] = 0;
    }

    // convert each individual character in the hex address
    for (i = 0; i < length; i++) {
        int num;
        char chr = hex[i];

        // do conversions based on hex letters
        if (chr == 'A' || chr == 'a') num = 10;
        else if (chr == 'B' || chr == 'b') num = 11;
        else if (chr == 'C' || chr == 'c') num = 12;
        else if (chr == 'D' || chr == 'd') num = 13;
        else if (chr == 'E' || chr == 'e') num = 14;
        else if (chr == 'F' || chr == 'f') num = 15;
        // now check for the numbers
        else if (chr == '0' || chr == '1' || chr == '2' ||
                chr == '3' || chr == '4' || chr == '5' ||
                chr == '6' || chr == '7' || chr == '8' ||
                chr == '9') num = (int)(chr) - 48;
        // if its a char that fits none of the above values, then it is an invalid
        // hex address and we make sure that valid is false
        else {
            valid = false;
            return valid;
        }

        // now we need to assign this to our temp conversion array
        // if the length is 4, then we assign to each corresponding slot in the array
        if (length == 4) {
            hexToIntConverted[i] = num;
        }
        // but if the length is 3, then we do one ahead of the current position because
        // the first value was already assigned to 0
        else {
            hexToIntConverted[i+1] = num;
        }
    }

    return valid;
}

/* Converts a decimal value to its hex equivalent
 * Parameters:
 *  dec - the decimal value to convert
 * Return value:
 *  none
 *  - BUT it will print out each of the individual in the total hex value
 */
void decToHex(int dec) {

    // declare all values to be used
    int quot, rem, i, j;
    j = 0;

    quot = dec;
    // iterate until we get to the "end" of the decimal value
    while (quot != 0) {
        rem = quot % 16;
        // if the remainder is less than 10, then it is a normal digit
        if (rem < 10) {
            intToHex[j] = 48 + rem;
            j++;
        }
        // else it is a letter digit and needs to be represented as such
        else {
            intToHex[j] = 55 + rem;
            j++;
        }
        quot /= 16; // decrease the quot so that we don't go into an infinite loop
    }

    // print it out in REVERSE order than how we stored it
    for (i = j; i >= 0; i--) {
        printf("%c", intToHex[i]);
    }
    printf("\n");

    return;

}