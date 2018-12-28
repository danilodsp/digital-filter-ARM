clear
close all
clc

%% Initializing the serial communication
    
s = serial('COM3','BaudRate',115200,'InputBufferSize',8*1024,'OutputBufferSize',8*1024,'Timeout',10);
fopen(s);

%% Creating the signal

Fs = 100000;
N = 1000;
t = 1/Fs:1/Fs:N/Fs;
u1 = sin(2*pi*10*t);
u2 = sin(2*pi*1000*t);
u3 = sin(2*pi*60*t);
ruido = 0.3*randn(1,N);
sinal2 = u2;

%% Design filter

% filtro = designfilt('bandpassiir','FilterOrder',20, ...
%     'CutoffFrequency1',1000,'CutoffFrequency2',2000, ...
%     'SampleRate',10000);

%[z,p,k] = butter(20,[2*pi*1000 2*pi*2000]/(2*pi*5000),'bandpass');
%[z,p,k] = butter(20,[2*pi*1 2*pi*2]/(2*pi*5),'bandpass');
[z,p,k] = butter(20,[2*pi*1000 2*pi*2000]/(2*pi*50000),'bandpass');
[z,p,k] = butter(20,[2*pi*10 2*pi*40]/(2*pi*700),'bandpass');
SOS = zp2sos(z,p,k); 
[b,a] = sos2tf(SOS);

% SOS = Hd.sosMatrix;
% G = Hd.ScaleValues;

%% Send and reception of the signal

nSamples = 1000;

bytes_adc = [SOS(:,4)' SOS(:,5)' SOS(:,6)' SOS(:,1)' SOS(:,2)' SOS(:,3)'];	
%bytes = [SOS(:,4)' SOS(:,5)' SOS(:,6)' SOS(:,1)' SOS(:,2)' SOS(:,3)' sinal];

fwrite(s,bytes_adc,'float');
%fwrite(s,bytes,'float');
y_arm = fread(s,nSamples,'float');

%% Perform the filter in Matlab

[d1,d2] = size(SOS);
y_matlab = zeros(1,1000);
w = zeros(d1,1000);
aux = sinal;

for i=1:1000
    if i>2 %%%%%%%%%%%%%%%%%%%%%%%%%
        for s=1:d1
            w(s,i) = SOS(s,4)*aux(i) - SOS(s,5)*w(s,i-1) - SOS(s,6)*w(s,i-2);
            aux(i) = SOS(s,1)*w(s,i) + SOS(s,2)*w(s,i-1) + SOS(s,3)*w(s,i-2);
        end
        y_matlab(i) = aux(i);
    elseif i==2 %%%%%%%%%%%%%%%%%%%%%%%%%
        for s=1:d1
            if(s==1)
                aux(i) = sinal(i);
            end
            w(s,i) = SOS(s,4)*aux(i) - SOS(s,5)*w(s,i-1);
            aux(i) = SOS(s,1)*w(s,i) + SOS(s,2)*w(s,i-1);
        end
        y_matlab(i) = aux(i);
    else %%%%%%%%%%%%%%%%%%%%%%%%%
        for s=1:d1
            if(s==1)
                aux(i) = sinal(i);
            end
            w(s,i) = SOS(s,4)*aux(i);
            aux(i) = SOS(s,1)*w(s,i);
        end
        y_matlab(i) = aux(i);
    end
end

%% Results analysis

plot((1/N)*abs(fft(sinal)))

% y = filter(Hd,sinal);
y = sosfilt(SOS,sinal2);

plot((1/N)*abs(fft(y_arm)))
plot((1/N)*abs(fft(y_matlab)))
plot((1/N)*abs(fft(y)))

% plot(sinal(100:200),'b')
% hold on
% plot(y(100:200),'r')

NFFT = 2^nextpow2(N);
Y = fft(y_arm,NFFT)/N;
f = Fs/2*linspace(0,1,NFFT/2+1);
plot(f,20*log10(2*abs(Y(1:NFFT/2+1))))

%% Finishing the serial communication

fclose(s);
delete(s)
clear s