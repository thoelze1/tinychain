n=int(input())
o=print
p=[1]
r=2 
i=0
o(1)
while r<=n:
  p+=[(i>0 and p[-r])+(i<r-1 and p[-r+1])]
  o(p[-1],end=' ')
  i+=1
  if i==r:
    i=0
    r+=1
    o()
