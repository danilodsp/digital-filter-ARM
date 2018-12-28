%% Initializing the serial communication

clear

s = serial('COM3','BaudRate',115200,'InputBufferSize',8*1024,'OutputBufferSize',8*1024,'Timeout',10);
fopen(s);

%% Creating the signal

Fs = 10000;
N = 1000;
t = 1/Fs:1/Fs:0.1;
u1 = sin(2*pi*1500*t);
u2 = sin(2*pi*500*t);
u3 = sin(2*pi*2500*t);
ruido = 0.3*randn(1,Fs/10);
sinal = u1 + u2 + u3 + ruido;

%% Send and receive the signal

clear R

[z,p,k] = butter(20,[2*pi*1000 2*pi*2000]/(2*pi*5000),'bandpass');
SOS = zp2sos(z,p,k);

%SOS = Hd.sosMatrix;
%G = Hd.ScaleValues;

%coeff = [SOS(:,4)' SOS(:,5)' SOS(:,6)' SOS(:,1)' SOS(:,2)' SOS(:,3)'];
coeff = [SOS(:,4)' SOS(:,5)' SOS(:,6)' SOS(:,1)' SOS(:,2)' SOS(:,3)' sinal];

fwrite(s,coeff,'float');
R = fread(s,1000,'float');

%plot(R)

%% Filter

[d1,d2] = size(SOS);
yn = zeros(1,1000);
w = zeros(d1,1000);
aux = zeros(1,1000);

aux = sinal;
for i=1:1000
    if i>2 %%%%%%%%%%%%%%%%%%%%%%%%%
        for s=1:d1
            w(s,i) = SOS(s,4)*aux(i) - SOS(s,5)*w(s,i-1) - SOS(s,6)*w(s,i-2);
            aux(i) = SOS(s,1)*w(s,i) + SOS(s,2)*w(s,i-1) + SOS(s,3)*w(s,i-2);
        end
        yn(i) = aux(i);
    elseif i==2 %%%%%%%%%%%%%%%%%%%%%%%%%
        for s=1:d1
            if(s==1)
                aux(i) = sinal(i);
            end
            w(s,i) = SOS(s,4)*aux(i) - SOS(s,5)*w(s,i-1);
            aux(i) = SOS(s,1)*w(s,i) + SOS(s,2)*w(s,i-1);
        end
        yn(i) = aux(i);
    else %%%%%%%%%%%%%%%%%%%%%%%%%
        for s=1:d1
            if(s==1)
                aux(i) = sinal(i);
            end
            w(s,i) = SOS(s,4)*aux(i);
            aux(i) = SOS(s,1)*w(s,i);
        end
        yn(i) = aux(i);
    end
end

plot((1/N)*abs(fft(sinal)))

y = filter(Hd,sinal);
plot((1/N)*abs(fft(yn)))

plot(sinal(100:200),'b')
hold on
plot(y(100:200),'r')

NFFT = 2^nextpow2(N);
Y = fft(R,NFFT)/N;
f = Fs/2*linspace(0,1,NFFT/2+1);
plot(f,20*log10(2*abs(Y(1:NFFT/2+1))))

%% Finishing the communication

fclose(s)
delete(s)
clear s