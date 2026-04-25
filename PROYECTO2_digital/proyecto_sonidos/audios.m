clear; clc; close all;

%% ---------------- CONFIG ----------------
archivo_audio = 'corazon.mp3';        % tu archivo MP3
nombre_array  = 'audio_corazon';    % nombre del arreglo en C
Fs_objetivo   = 8000;               % igual que tu TIM6
ganancia      = 1;                % volumen digital
umbral        = 20;                 % para detectar silencio cerca de 2048

%% ---------------- LEER AUDIO ----------------
[x, Fs] = audioread(archivo_audio);

% Si viene estereo, pasarlo a mono
if size(x,2) == 2
    x = mean(x, 2);
end

%% ---------------- REMUESTREAR ----------------
if Fs ~= Fs_objetivo
    x = resample(x, Fs_objetivo, Fs);
    Fs = Fs_objetivo;
end

%% ---------------- AJUSTAR AMPLITUD ----------------
x = ganancia * x;

% Limitar por seguridad
x(x > 1)  = 1;
x(x < -1) = -1;

%% ---------------- CONVERTIR A DAC 12 BITS ----------------
% x está entre -1 y 1
% dac quedará entre 0 y 4095
dac = round((x + 1) * (4095/2));

% asegurar límites
dac(dac < 0)    = 0;
dac(dac > 4095) = 4095;

%% ---------------- RECORTAR SILENCIO ----------------
centro = 2048;
idx_activos = find(abs(dac - centro) > umbral);

if isempty(idx_activos)
    error('Todo el audio quedó como silencio. Prueba bajando el umbral.');
end

primero = idx_activos(1);
ultimo  = idx_activos(end);

dac_recortado = dac(primero:ultimo);

%% ---------------- MOSTRAR INFO ----------------
fprintf('Fs final: %d Hz\n', Fs);
fprintf('Muestras originales: %d\n', length(dac));
fprintf('Muestras recortadas: %d\n', length(dac_recortado));
fprintf('Duracion final: %.3f s\n', length(dac_recortado)/Fs);

%% ---------------- GRAFICAS ----------------
figure;
plot(dac);
title('Audio completo en escala DAC');
xlabel('Muestra');
ylabel('Nivel DAC');

figure;
plot(dac_recortado);
title('Audio recortado');
xlabel('Muestra');
ylabel('Nivel DAC');

%% ---------------- GUARDAR EN TXT FORMATO C ----------------
nombre_txt = [nombre_array '.txt'];
fid = fopen(nombre_txt, 'w');

fprintf(fid, 'const uint16_t %s[] = {\n', nombre_array);

for i = 1:length(dac_recortado)
    if mod(i-1, 12) == 0
        fprintf(fid, '    ');
    end

    if i < length(dac_recortado)
        fprintf(fid, '%d, ', dac_recortado(i));
    else
        fprintf(fid, '%d', dac_recortado(i));
    end

    if mod(i, 12) == 0
        fprintf(fid, '\n');
    end
end

if mod(length(dac_recortado), 12) ~= 0
    fprintf(fid, '\n');
end

fprintf(fid, '};\n\n');
fprintf(fid, '#define LEN_%s %d\n', upper(nombre_array), length(dac_recortado));

fclose(fid);

fprintf('\nArchivo generado: %s\n', nombre_txt);