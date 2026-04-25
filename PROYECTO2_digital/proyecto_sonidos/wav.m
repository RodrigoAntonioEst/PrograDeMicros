clear; clc; close all;

%% ---------------- CONFIG ----------------
archivo_audio = 'fondo.wav';        % archivo de entrada
nombre_array  = 'audio_back';       % nombre del arreglo en C
Fs_objetivo   = 8000;               % igual que tu TIM6
ganancia      = 1.0;                % volumen digital
umbral        = 20;                 % para detectar silencio cerca de 2048

% rango donde esperas encontrar UN ciclo
% ajusta estos dos si el periodo detectado sale mal
periodo_min = 500;                  % minimo de muestras por ciclo
periodo_max = 12000;                % maximo de muestras por ciclo

% ventana para ajustar mejor el inicio del ciclo
ventana_ajuste = 400;

%% ---------------- LEER AUDIO ----------------
[x, Fs] = audioread(archivo_audio);

% si viene estéreo, pasarlo a mono
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

% limitar por seguridad
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
    error('Todo el audio quedó como silencio. Baja el umbral.');
end

primero = idx_activos(1);
ultimo  = idx_activos(end);
dac_recortado = dac(primero:ultimo);

%% ---------------- DETECTAR PERIODO CON AUTOCORRELACION ----------------
y = double(dac_recortado);
y0 = y - 2048;   % quitar offset DC para analizar mejor

[r, lags] = xcorr(y0, 'coeff');

% quedarnos solo con lags positivos
idx_pos = lags > 0;
r_pos   = r(idx_pos);
lags_pos = lags(idx_pos);

% limitar la búsqueda del periodo
idx_busq = find(lags_pos >= periodo_min & lags_pos <= periodo_max);

if isempty(idx_busq)
    error('No hay lags dentro del rango de busqueda. Ajusta periodo_min y periodo_max.');
end

r_busq    = r_pos(idx_busq);
lags_busq = lags_pos(idx_busq);

% lag con mayor correlación = periodo estimado
[~, kmax] = max(r_busq);
periodo = lags_busq(kmax);

fprintf('Periodo estimado: %d muestras\n', periodo);
fprintf('Duracion aproximada de un ciclo: %.4f s\n', periodo/Fs);

%% ---------------- AJUSTAR MEJOR EL INICIO DEL CICLO ----------------
% buscamos un inicio donde y(i) se parezca mucho a y(i+periodo)
mejor_err = inf;
mejor_i = 1;

limite_busqueda = min(ventana_ajuste, length(y) - periodo);

for i = 1:limite_busqueda
    err = abs(y(i) - y(i + periodo));
    if err < mejor_err
        mejor_err = err;
        mejor_i = i;
    end
end

inicio = mejor_i;
fin    = inicio + periodo - 1;

if fin > length(y)
    error('El ciclo estimado se sale del arreglo. Ajusta parametros.');
end

ciclo = y(inicio:fin);

fprintf('Inicio elegido: %d\n', inicio);
fprintf('Fin elegido: %d\n', fin);
fprintf('Error inicio-fin: %.2f\n', mejor_err);

%% ---------------- GRAFICAS ----------------
figure;
plot(dac);
grid on;
title('Audio completo en escala DAC');
xlabel('Muestra');
ylabel('Nivel DAC');

figure;
plot(dac_recortado);
grid on;
title('Audio recortado (sin silencios grandes)');
xlabel('Muestra');
ylabel('Nivel DAC');

figure;
plot(lags_busq, r_busq);
grid on;
title('Autocorrelacion en rango de busqueda');
xlabel('Lag (muestras)');
ylabel('Correlacion');

figure;
plot(ciclo);
grid on;
title('Un ciclo detectado');
xlabel('Muestra');
ylabel('Nivel DAC');

%% ---------------- INFO FINAL ----------------
fprintf('Fs final: %d Hz\n', Fs);
fprintf('Muestras originales: %d\n', length(dac));
fprintf('Muestras recortadas: %d\n', length(dac_recortado));
fprintf('Muestras del ciclo: %d\n', length(ciclo));

%% ---------------- GUARDAR EN TXT FORMATO C ----------------
nombre_txt = [nombre_array '.txt'];
fid = fopen(nombre_txt, 'w');

fprintf(fid, 'const uint16_t %s[] = {\n', nombre_array);

for i = 1:length(ciclo)
    if mod(i-1, 12) == 0
        fprintf(fid, '    ');
    end

    if i < length(ciclo)
        fprintf(fid, '%d, ', round(ciclo(i)));
    else
        fprintf(fid, '%d', round(ciclo(i)));
    end

    if mod(i, 12) == 0
        fprintf(fid, '\n');
    end
end

if mod(length(ciclo), 12) ~= 0
    fprintf(fid, '\n');
end

fprintf(fid, '};\n\n');
fprintf(fid, '#define LEN_%s %d\n', upper(nombre_array), length(ciclo));

fclose(fid);

fprintf('\nArchivo generado: %s\n', nombre_txt);