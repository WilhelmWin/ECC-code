typedef long long int lli;

typedef lli gf[16];

typedef unsigned char uch;

#define sv static void

#define sq(o,i) mul(o,i,i)

static gf _121665 = {0xDB41,1};

static const uch base[32] = {9};

sv car(gf o)
{
    int i;
    lli c;
    for(i=0;i<16;i++)
    {
        o[i]+=(1<<16);
        c = o[i]>>16;
        o[(i+1)*(i<15)] +=
        c-1 + 37*(c-1)*(i==15);
        o[i] -= c<<16;
    }
}
sv add(gf o,gf a,gf b)
{
    int i;
    for(i=0;i<16;i++)
        o[i]=a[i]+b[i];
}
sv sub(gf o,gf a,gf b)
{
    int i;
    for(i=0;i<16;i++)
        o[i]=a[i]-b[i];
}
sv mul(gf o,gf a,gf b)
{
    lli i,j,c[31];
    for(i=0;i<31;i++)
        c[i] = 0;
    for(i=0;i<16;i++)
        for(j=0;j<16;j++)
            c[i+j] += a[i] * b[j];
    for(i=16;i<31;i++)
        c[i-16] += 38*c[i];
    for(i=0;i<16;i++)
        o[i] = c[i];
    car(o);
    car(o);
}
sv inv(gf o,gf i)
{
    gf c;
    int a;
    for(a=0;a<16;a++)
        c[a]=i[a];
    for(a=253;a>=0;a--)
    {
        sq(c,c);
        if(a!=2&&a!=4)
            mul(c,c,i);
    }
    for(a=0;a<16;a++)
        o[a]=c[a];
}
sv sel(gf p,gf q,int b)
{
    lli t,u,i,b1=~(b-1);
    for (i=0;i<16;i++)
    {
        t = b1 & (p[i]^q[i]);
        p[i] ^= t;
        q[i] ^= t;
    }
}
sv mainloop(lli x[32],uch *z)
{
    gf a,b,c,d,e,f;
    lli p,i;
    for(i=0;i<16;i++)
    {
        b[i] = x[i];
        d[i] = a[i] = c[i] = 0;
    }
    21
    a[0] = d[0] = 1;
    for(i=254;i>=0;--i)
    {
        p = (z[i>>3] >> (i&7))&1;
        sel(a,b,p);
        sel(c,d,p);
        add(e,a,c);
        sub(a,a,c);
        add(c,b,d);
        sub(b,b,d);
        sq(d,e);
        sq(f,a);
        mul(a,c,a);
        mul(c,b,e);
        add(e,a,c);
        sub(a,a,c);
        sq(b,a);
        sub(c,d,f);
        mul(a,c,_121665);
        add(a,a,d);
        mul(c,c,a);
        mul(a,d,f);
        mul(d,b,x);
        sq(b,e);
        sel(a,b,p);
        sel(c,d,p);
    }
    for(i=0;i<16;i++)
    {
        x[i] = a[i];
        x[i+16] = c[i];
    }
}
sv unpack(gf o,const uch *n)
{
    int i;
    for(i=0;i<16;i++)
        o[i] = n[2*i] +
        ((lli)n[2*i+1]<<8);
}
sv pack(uch *o,gf n)
{
    int i,j,b;
    gf m;
    car(n);
    car(n);
    car(n);
    for(j=0;j<2;j++)
    {
        m[0] = n[0] - 0xffed;
        for(i=1;i<15;i++)
        {
            m[i] = n[i] - 0xffff -
            ((m[i-1] >> 16)&1);
            m[i-1] &= 0xffff;
        }
        m[15] = n[15] - 0x7fff -
        ((m[14] >> 16)&1);
        b = (m[15] >> 16)&1;
        m[14] &= 0xffff;
        sel(n,m,1-b);
    }
    for(i=0; i<16; i++)
    {
        o[2*i] = n[i]&0xff;
        o[2*i+1] = n[i]>>8;
    }
}
int crypto_scalarmult(uch *q,
const uch *n,const uch *p)
{
    uch z[32];
    lli x[32];
    int i;
    for(i = 0;i < 31;++i)
        z[i] = n[i];
    z[31] = (n[31] & 127) | 64;
    z[0] &= 248;
    unpack(x,p);
    mainloop(x,z);
    inv(x+16,x+16);
    mul(x,x,x+16);
    pack(q,x);
    return 0;
}
int crypto_scalarmult_base(uch *q,
const uch *n)
{
    return crypto_scalarmult(q,n,base);
}