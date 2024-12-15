clc;
clear;

% Longitudes de los eslabones del robot
L1 = 3.2;   % Base a hombro
L2 = 2.1;   % Hombro a codo
L3 = 10.05; % Codo a muñeca
L4 = 8.2;   % Muñeca al efector
L5 = 4.3;   % Longitud del efector final

% Definición de los eslabones (DH modificado)
L(1) = Revolute('d', L1, 'a', 0,    'alpha', pi/2, 'offset', 0); % Servo 1
L(2) = Revolute('d', 0,  'a', 0,    'alpha', pi/2, 'offset', 0); % Servo 2
L(3) = Revolute('d', 0,  'a', L3,   'alpha', 0,    'offset', 0); % Servo 3
L(4) = Revolute('d', 0,  'a', L4,   'alpha', pi/2, 'offset', 0); % Servo 4
L(5) = Revolute('d', 0,  'a', 0,    'alpha', -pi/2,'offset', 0); % Servo 5
L(6) = Revolute('d', L5, 'a', 0,    'alpha', 0,    'offset', 0); % Servo 6

% Creación del robot
robot6GDL = SerialLink([L(1), L(2), L(3), L(4), L(5), L(6)], 'name', 'Brazo 6GDL');

% Coordenadas objetivo para los servomotores en grados
q_obj = [45, 0, 11.69, 0, 0, 0]; % Servo 1 a 6

% Conversión de grados a radianes
q_rad = deg2rad(q_obj);

% Visualización del robot
figure;
title('Simulación del Robot 6GDL - Configuración Objetivo');
robot6GDL.plot(q_rad, 'workspace', [-20 20 -20 20 0 30], 'scale', 0.5);
grid on;

% Imprimir los ángulos objetivo en la consola
fprintf('Valores Objetivo de los Servos (grados):\n');
fprintf('Servo 1: %.2f°\n', q_obj(1));
fprintf('Servo 2: %.2f°\n', q_obj(2));
fprintf('Servo 3: %.2f°\n', q_obj(3));
fprintf('Servo 4: %.2f°\n', q_obj(4));
fprintf('Servo 5: %.2f°\n', q_obj(5));
fprintf('Servo 6: %.2f°\n', q_obj(6));
