
int gcd(int a,int b){
    if(a==0||b==0)return a+b;
    else return gcd(b%a,a);
}

int main(){
    return gcd(24,42);
}