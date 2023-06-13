// Sample Objective-C code that sorts an array of strings.
// The array is sorted in place, and the sorted array is returned.
// The sort is case-insensitive.

#import <Foundation/Foundation.h>

// let's add a function that takes array as input and returns sorted array
NSArray* sortArray(NSArray* array) {
    return [array sortedArrayUsingSelector:@selector(caseInsensitiveCompare:)];
}

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // lets' call sortArray function
    NSArray *array = [NSArray arrayWithObjects:
        @"This", @"is", @"a", @"test", @"of", @"the", @"sorting", @"algorithm", nil];
    NSArray *sortedArray = sortArray(array);


    // NSArray *array = [NSArray arrayWithObjects:
    //     @"This", @"is", @"a", @"test", @"of", @"the", @"sorting", @"algorithm", nil];
    // NSArray *sortedArray;
    // sortedArray = [array sortedArrayUsingSelector:@selector(caseInsensitiveCompare:)];

    NSLog(@"sortedArray: %@", sortedArray);

    [pool drain];
    return 0;
}
