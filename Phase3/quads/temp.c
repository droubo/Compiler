#define INT_MIN -2147483647
#define INT_MAX 2147483647

int numPlaces (int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

/* Used code from Lecture 9 Slide 45 */
int tempcounter = 0;
newtempname() {
    

}
