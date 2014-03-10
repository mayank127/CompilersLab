main()
{
	

float prev1=0,prev2=1,next,n=1;

do
{
	
	next=prev1+prev2;
	prev1=prev2;	
	prev2=next;
		
	n=n+1;	

}while(n<5);

}
