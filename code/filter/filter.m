% Filter
clear

s = serial('COM3','BaudRate',115200,'InputBufferSize',8*512,'OutputBufferSize',8*512,'Timeout',10);
fopen(s);

%%

u = [zeros(1,50) ones(1,50)];
ruido = 0.1*randn(1,100);

sinal = u+ruido;

%%
%fprintf(s,'oi');
%r = fscanf(s)

fwrite(s,sinal,'double');
R = fread(s,[1,100],'double');

plot(R)



%%
fclose(s)
delete(s)
clear s