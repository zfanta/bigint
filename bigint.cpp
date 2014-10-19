#include <iostream>
#include <fstream>
#include <string>  
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
 
#pragma warning(disable:4996)
 
class bigint
{
private:
	char sign;		//+:0, -:1					
	unsigned *dats;	//가장 뒷자리가 dats[0]에 저장
	unsigned len;	//dats배열 길이
 
public:
	unsigned cona2u(char*);
 
	bigint();
	~bigint();
	bigint(const int);
	bigint(const unsigned);
	bigint(const char*);
	bigint(const bigint &);
 
	unsigned resize(unsigned);	//길이조절
	unsigned resize();			//앞의 0 지우기
	bigint abs();				//절대값
	bigint negative(unsigned);	//1의보수	
 
	int cmp(bigint &b);			//strcmp와 리턴값 같음
 
	bigint _or(bigint &b);		//this에 저장
	bigint _and(bigint &b);		//this에 저장
	bigint _xor(bigint &b);		//this에 저장
	bigint shr(unsigned n);		//this에 (this>>n)을 저장
	bigint shl(unsigned n);		//this에 (this<<n)을 저장	
 
	bigint add(bigint &b);		//(this+b)를 리턴
	bigint sub(bigint &b);		//(this-b)를 리턴
	bigint mul(bigint &b);		//(this*b)를 리턴
	bigint div(bigint &b, int);	//(this/b)또는 (this%b)리턴
 
	std::string conbcd();		//10진
 
	bigint operator=(const int);
	bigint operator=(const bigint &);
	bigint operator=(const char*);
 
	friend bigint operator-(bigint);
 
	friend bigint operator|(bigint, bigint);
	friend bigint operator&(bigint, bigint);
	friend bigint operator^(bigint, bigint);
	friend bigint operator+(bigint, bigint);
	friend bigint operator-(bigint, bigint);
	friend bigint operator*(bigint, bigint);
	friend bigint operator/(bigint, bigint);
	friend bigint operator%(bigint, bigint);
	friend bigint operator<<(bigint, bigint);
	friend bigint operator>>(bigint, bigint);
 
	friend bool operator<(bigint, bigint);
	friend bool operator<=(bigint, bigint);
	friend bool operator>(bigint, bigint);
	friend bool operator>=(bigint, bigint);
	friend bool operator==(bigint, bigint);
	friend bool operator!=(bigint, bigint);
 
	bigint operator|=(bigint);
	bigint operator&=(bigint);
	bigint operator^=(bigint);
	bigint operator+=(bigint);
	bigint operator-=(bigint);
	bigint operator*=(bigint);
	bigint operator/=(bigint);
	bigint operator%=(bigint);
	bigint operator<<=(bigint);
	bigint operator>>=(bigint);
 
	bigint operator++();
	bigint operator++(int dummy);
	bigint operator--();
	bigint operator--(int dummy);
 
	friend std::ostream& operator <<( std::ostream& os, bigint& b );
	friend std::istream& operator >>( std::istream& is, bigint& b );
};
 
unsigned bigint::cona2u(char *src)
{
	unsigned ten, sum, eos=strlen(src)-1;
 
	for(sum=0, ten=1; (int)eos>=0 && ten<=100000000; ten*=10, eos--)
	{
		sum+=ten*(src[eos]-'0');
		src[eos]=0;
	}
	return sum;
}
 
bigint::bigint()
{
	len=1;
	sign=0;
	dats=(unsigned*)calloc(1,sizeof(unsigned));
}
 
bigint::~bigint()
{
	free(dats);
}
 
bigint::bigint(const int src)
{
	len=1;
	dats=(unsigned*)malloc(sizeof(unsigned)*len);
	if(src<0)
	{
		sign=1;
		dats[0]=-src;
	}
	else
	{
		sign=0;
		dats[0]=src;
	}
}
 
bigint::bigint(const char *src)
{
	char *str=(char*)malloc(sizeof(char)*(strlen(src)+1));
 
	sign=0;
	strcpy(str, src);
	if(src[0]=='-')
		strcpy(str,src+1), sign=1;
	if(src[0]=='+')
		strcpy(str,src+1);
 
	len=(strlen(src)-1)/9 + 1;
	dats=(unsigned*)malloc(sizeof(unsigned)*len);
	unsigned i;
	for(i=0; i<len; i++)
	{
		dats[i]=cona2u(str);
	}
	free(str);
	shl(len*32);
 
	unsigned j, lmt=len/2;
	for(i=(len/2)*32; i; i--)
	{
		shr(1);
		for(j=len-1; j>=lmt; j--)
		{
			if(dats[j]>=2147483648)
				dats[j]-=1647483648;
		}
	}
}
 
bigint::bigint(const bigint &src)
{
	len=src.len;
	sign=src.sign;
	dats=(unsigned*)malloc(sizeof(unsigned)*len);
	memcpy(dats, src.dats, sizeof(unsigned)*len);
}
 
unsigned bigint::resize(unsigned len_new)
{
	if(len==len_new)
		return len;
	unsigned *new_p=(unsigned*)malloc(sizeof(unsigned)*len_new);		//
	memcpy(new_p, dats, sizeof(unsigned)*((len<len_new)?len:len_new));	//이부분에 realloc(dats, sizeof(unsigned)*len_new)쓰면 왜 값이 바뀌나요
	free(dats);															//
	dats=new_p;															//
 
	if(len < len_new)
		memset(dats+len, 0, sizeof(unsigned)*(len_new-len));
	len=len_new;
	return len;
}
 
unsigned bigint::resize()
{
	unsigned i;
	for(i=len-1; dats[i]==0; i--);
	i++;
	if(i)
		resize(i);
	else
		resize(1);
	return len;
}
 
bigint bigint::abs()
{
	bigint toreturn(*this);
	toreturn.sign=0;
	return toreturn;
}
 
//1의 보수
bigint bigint::negative(unsigned len_new)
{
	bigint toreturn(*this);
	toreturn.resize(len_new);
	for(unsigned i=0; i<len_new; i++)
		toreturn.dats[i]=~toreturn.dats[i];
	return toreturn;
}
 
int bigint::cmp(bigint &b)
{
	if(len==1 && b.len==1 && dats[0]==0 && b.dats[0]==0)
		return 0;
	if(sign==1 && b.sign==0)
		return -1;
	if(sign==0 && b.sign==1)
		return 1;
 
	int left=1, right=-1;	
	if(sign==1 && b.sign==1)
		left=-1, right=1;
 
	if(len>b.len)
		return left;
	if(len<b.len)
		return right;
 
	for(int i=len-1; i>=0; i--)
	{
		if(dats[i]>b.dats[i])
			return left;
		if(dats[i]<b.dats[i])
			return right;
	}
	return 0;
}
 
bigint bigint::_or(bigint &b)
{
	if(len<b.len)
		resize(b.len);
	unsigned i;
	for(i=0; i<b.len; i++)
		dats[i]|=b.dats[i];
	return *this;
}
 
bigint bigint::_and(bigint &b)
{
	if(len<b.len)
		resize(b.len);
	unsigned i;
	for(i=0; i<b.len; i++)
		dats[i]&=b.dats[i];
	return *this;
}
 
bigint bigint::_xor(bigint &b)
{
	if(len<b.len)
		resize(b.len);
	unsigned i;
	for(i=0; i<b.len; i++)
		dats[i]^=b.dats[i];
	return *this;
}
 
bigint bigint::shr(unsigned n)
{
	unsigned move=n>>5, cut=n&31, i, j, tmp;
	dats[0]=dats[move]>>cut;
	for(i=1; i<len-move; i++)
	{
		for(j=0, tmp=dats[i+move]; j<32-cut; j++)	//dats[i+1]=dats[i+1]|(dats[i+move]<<(32-cut));
			tmp<<=1;								//
		dats[i-1]|=tmp;
		dats[i]=dats[i+move]>>cut;  
	}
 
 
	resize(len-move);//옮겨진 부분 삭제
	resize();
	return *this;
}
 
bigint bigint::shl(unsigned n)
{
	unsigned move=n>>5, cut=n&31, i, j, tmp;
 
	resize(len+move+1);
	dats[len-1]=dats[len-move-1]<<cut;
	for(i=len-2; (int)i>=(int)move; i--)
	{
		for(j=0, tmp=dats[i-move]; j<32-cut; j++)	//dats[i+1]=dats[i+1]|(dats[i-move]>>(32-cut));
			tmp>>=1;								//
		dats[i+1]=dats[i+1]|tmp;					//
		dats[i]=dats[i-move]<<cut;
	}
	memset(dats, 0, sizeof(unsigned)*move);//뒤에 옮겨진 부분 0으로
 
	resize();
	return *this;
}
 
bigint bigint::add(bigint &b)
{
	bigint toreturn=*this;
	long long carry=0;
	unsigned i;
	toreturn.resize( ((b.len>toreturn.len)?b.len:toreturn.len) + 1 );
 
	for(i=0; i<b.len; i++)
	{
		carry = (long long)toreturn.dats[i] + (long long)b.dats[i] + carry;
		toreturn.dats[i]=carry&(((long long)1<<32)-1);
		carry>>=32;
	}
 
	for(; carry; i++)
	{
		carry=(long long)toreturn.dats[i] + carry;
		toreturn.dats[i]=carry&(((long long)1<<32)-1);
		carry>>=32;
	}
 
	toreturn.resize();
 
	return toreturn;
}
 
//1의 보수 사용
bigint bigint::sub(bigint &b)
{
	bigint toreturn(*this);
	bigint subtrahend(b);	//감수
 
	unsigned len_new=((toreturn.len>b.len)?toreturn.len:b.len);
	subtrahend=subtrahend.negative(len_new);
 
	toreturn=toreturn.add(subtrahend);
	if(toreturn.len > len_new)//자리올림수있으면
	{
		toreturn.resize(len_new);
		toreturn=toreturn.add((bigint)1);
	}
	else//자리올림수 없으면
	{
		toreturn=toreturn.negative(len_new);
		toreturn.sign=1;
	}
	return toreturn;
}
 
bigint bigint::mul(bigint &b)
{
	bigint result, tmp;
	unsigned i, j;
	unsigned long long carry;
	for(i=0; i<b.len; i++)
	{
		tmp.resize(len+1+i);
		for(carry=j=0; j<len; j++)
		{
			carry=(long long)dats[j]*(long long)b.dats[i]+carry;
			tmp.dats[j+i]=(unsigned)carry&(((long long)1<<32)-1);
			carry>>=32;
		}
		tmp.dats[j+i]=(unsigned)carry;
		result=result.add(tmp);
		memset(tmp.dats, 0, sizeof(unsigned)*tmp.len);
	}
	result.resize();
	return result;	
}
 
bigint bigint::div(bigint &b, int ask_mod=0)
{
	bigint r=abs(), q=0, divisor=b.abs(), one=1;
	unsigned i;
 
	for(i=0;divisor.cmp(r)<=0;divisor.shl(1),i++);
	i--;
	if((int)i>=0)
	{
		one.shl(i);
		divisor.shr(1);
 
		while(b.cmp(r)<=0)//b<=r
		{
			for(;divisor.cmp(r)>0;divisor.shr(1),one.shr(1));
			q._or(one);
			r=r.sub(divisor);
		}
	}
 
	if(ask_mod)
		return r;
	return q;
}
 
std::string bigint::conbcd()
{
	unsigned *bcd, len_new, head, i, j;
	char  str_tmp[10];
	std::string result;
 
	len_new=10704*len/10000+1+len;
 
	bcd=(unsigned*)calloc(len_new, sizeof(unsigned));
	memcpy(bcd, dats, sizeof(unsigned)*len);
 
	for(head=len, i=0; i<32*len; i++)
	{
		for(j=head; j>=len; j--)
		{
			if(bcd[j]>=500000000)
				bcd[j]+=1647483648;
		}
		if(bcd[head]>>31)
			head++;
		bcd[head]<<=1;
		for(j=head-1; (int)j>=0; j--)
		{
			bcd[j+1]|=(bcd[j]>>31);
			bcd[j]<<=1;
		}		
	}
 
	for(i=len_new-1; bcd[i]==0; i--);
 
	if((int)i<0)
	{
		result="0";
		return result;
	}
 
	sprintf(str_tmp, "%u", bcd[i]);
	result.insert(result.size(), str_tmp);
 
	for(--i; (int)i>=len; i--)
	{
		sprintf(str_tmp, "%09u", bcd[i]);
		result.insert(result.size(), str_tmp);
	}
 
	free(bcd);
	return result;
}
 
bigint bigint::operator=(const int src)
{
	resize(1);
	sign=0;	
	dats[0]=src;
	if(src<0)
		sign=1, dats[0]*=-1;
	return *this;
}
 
bigint bigint::operator=(const bigint &src)
{
	resize(src.len);
	sign=src.sign;
	memcpy(dats, src.dats, sizeof(unsigned)*len);
	return *this;
}
 
bigint bigint::operator=(const char *src)
{
	char *str=(char*)malloc(sizeof(char)*(strlen(src)+1));
 
	sign=0;
	strcpy(str, src);
	if(src[0]=='-')
		strcpy(str,src+1), sign=1;
	if(src[0]=='+')
		strcpy(str,src+1);
 
	resize((strlen(src)-1)/9 + 1);
	unsigned i;
	for(i=0; i<len; i++)
	{
		dats[i]=cona2u(str);
	}
	free(str);
	shl(len*32);
 
	unsigned j, lmt=len/2;
	for(i=(len/2)*32; i; i--)
	{
		shr(1);
		for(j=len-1; j>=lmt; j--)
		{
			if(dats[j]>=2147483648)
				dats[j]-=1647483648;
		}
	}
 
	return *this;
}
 
bigint operator-(bigint a)
{
	a.sign^=1;
	return a;
}
 
bigint operator|(bigint a, bigint b)
{
	return a._or(b);
}
 
bigint operator&(bigint a, bigint b)
{
	return a._and(b);
}
 
bigint operator^(bigint a, bigint b)
{
	return a._xor(b);
}
 
bigint operator+(bigint a, bigint b)
{
	if(a.sign==b.sign)
		return a.add(b);
	if(a.sign==0 && b.sign==1)
		return a.sub(b);
	//a.sign==1 && b.sign==0
	return b.sub(a);
}
 
bigint operator-(bigint a, bigint b)
{
	if(a.sign==1 && b.sign==1)
	{
		b.sign=0;
		if((a.abs()).cmp(b.abs())==1)
			b.sign=1;
		b=b.sub(a);
		return b;
	}
	if(a.sign==0 && b.sign==0)
		return a.sub(b);
	if(a.sign==0 && b.sign==1)
		return a.add(b);
	//a.sign==1 && b.sign==0
	a=a.add(b);
	a.sign=1;
	return a;
}
 
bigint operator*(bigint a, bigint b)
{
	bigint toreturn=a.mul(b);
	toreturn.sign=a.sign^b.sign;
	return toreturn;
}
 
bigint operator/(bigint a, bigint b)
{
	bigint toreturn=a.abs().div(b.abs());
	toreturn.sign=a.sign^b.sign;
	return toreturn;
}
 
bigint operator%(bigint a, bigint b)
{
	bigint toreturn=a.abs().div(b.abs(),1);
	toreturn.sign=a.sign;
	return toreturn;
}
 
bigint operator<<(bigint a, bigint b)
{
	bigint tmp(2147483647);
	while(b.len>1)
	{
		a.shl(2147483647);
		b=b.sub(tmp);
	}
	a.shl(b.dats[0]);
	return a;
}
 
bigint operator>>(bigint a, bigint b)
{
	bigint tmp(2147483647);
	while(b.len>1)
	{
		a.shr(2147483647);
		b=b.sub(tmp);
	}
	a.shr(b.dats[0]);
	return a;
}
 
bool operator<(bigint a, bigint b)
{
	if(a.cmp(b)<0)
		return true;
	return false;
}
 
bool operator<=(bigint a, bigint b)
{
	if(a.cmp(b)<=0)
		return true;
	return false;
}
 
bool operator>(bigint a, bigint b)
{
	if(a.cmp(b)>0)
		return true;
	return false;
}
 
bool operator>=(bigint a, bigint b)
{
	if(a.cmp(b)>=0)
		return true;
	return false;
}
 
bool operator==(bigint a, bigint b)
{
	if(a.cmp(b)==0)
		return true;
	return false;
}
 
bool operator!=(bigint a, bigint b)
{
	if(a.cmp(b)==0)
		return false;
	return true;
}
 
bigint bigint::operator|=(bigint b)
{
	*this=*this|b;
	return *this;
}
 
bigint bigint::operator&=(bigint b)
{
	*this=*this&b;
	return *this;
}
bigint bigint::operator^=(bigint b)
{
	*this=*this^b;
	return *this;
}
 
bigint bigint::operator+=(bigint b)
{
	*this=*this+b;
	return *this;
}
 
bigint bigint::operator-=(bigint b)
{
	*this=*this-b;
	return *this;
}
 
bigint bigint::operator*=(bigint b)
{
	*this=*this*b;
	return *this;
}
 
bigint bigint::operator/=(bigint b)
{
	*this=*this/b;
	return *this;
}
 
bigint bigint::operator%=(bigint b)
{
	*this=*this%b;
	return *this;
}
 
bigint bigint::operator<<=(bigint b)
{
	*this=*this<<b;
	return *this;
}
 
bigint bigint::operator>>=(bigint b)
{
	*this=*this>>b;
	return *this;
}
 
bigint bigint::operator++()
{
	*this+=1;
	return *this;
}
 
bigint bigint::operator++(int dummy)
{
	bigint old(*this);
	*this+=1;
	return old;
}
 
bigint bigint::operator--()
{
	*this-=1;
	return *this;
}
 
bigint bigint::operator--(int dummy)
{
	bigint old(*this);
	*this-=1;
	return old;
}
 
std::ostream& operator<<(std::ostream& os, bigint& b)
{
	os<<((b.sign)?"-":"")<<b.conbcd();
 
	return os;
}
 
std::istream& operator>>(std::istream& is, bigint& b)
{
	std::string numstr;
	is>>numstr;
	b=numstr.c_str();
 
	return is;
}
 
bigint fac(bigint n)  
{
	bigint result=1;
	for(; n>0; n--)
		result*=n;
 
	return result;
}
 
int main()
{
	clock_t start, end;
 
	bigint n=1;
 
	std::cout<<">";
	std::cin>>n;
	while(n>0)
	{
		start=clock();
 
		std::cout<<n<<"!:\n";
		std::cout<<fac(n)<<'\n';
 
		end=clock();
		std::cout<<(double)(end-start) / CLOCKS_PER_SEC<<"sec\n";
 
		std::cout<<">";
		std::cin>>n;
	}
 
	return 0;
}