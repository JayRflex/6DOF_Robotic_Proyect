function simulacion_robot_6GDL_inversa()
    % Parametros del robot - Longitudes del brazo (cm)
    l1 = 3.2;  % Eslabon 1
    l2 = 2.1;  % Eslabon 2
    l3 = 10.05; % Eslabon 3 (hombro a codo)
    l4 = 8.2;  % Eslabon 4 (codo a muneca)
    l5 = 4.3;  % Eslabon 5 (muneca al efector final)

    % Crear el modelo del robot
    robot = rigidBodyTree('DataFormat', 'row', 'MaxNumBodies', 6);

    % Crear los cuerpos y articulaciones del robot
    addBody(robot, crearCuerpo('body1', 'joint1', 'revolute', [0 0 0], [0 1 0], [0 140]), 'base');
    addBody(robot, crearCuerpo('body2', 'joint2', 'revolute', [0 0 l1], [0 0 1], [0 140]), 'body1');
    addBody(robot, crearCuerpo('body3', 'joint3', 'revolute', [0 0 0], [1 0 0], [0 140]), 'body2');
    addBody(robot, crearCuerpo('body4', 'joint4', 'revolute', [0 0 l3], [1 0 0], [0 140]), 'body3');
    addBody(robot, crearCuerpo('body5', 'joint5', 'revolute', [0 0 l4], [1 0 0], [0 140]), 'body4');
    addBody(robot, crearCuerpo('body6', 'joint6', 'revolute', [0 0 l5], [0 0 1], [0 140]), 'body5');

    % Asociar el efector final al ultimo cuerpo
    efectorFinal = rigidBody('end_effector');
    setFixedTransform(efectorFinal.Joint, trvec2tform([0 0 0]));
    addBody(robot, efectorFinal, 'body6');

    % Crear figura para la simulacion
    fig = figure('Name', 'Simulacion del Robot 6DOF', 'Position', [100, 100, 1000, 600]);
    ax = axes('Parent', fig, 'Position', [0.2, 0.3, 0.6, 0.65]);
    axis(ax, 'equal');
    grid(ax, 'on');
    view(3);
    hold(ax, 'on');

    % Graficar la bancada elevada (base)
    graficarBancadaElevada(ax);

    % Valores iniciales
    jointValues = zeros(1, 6); % Todas las articulaciones en 0
    show(robot, jointValues, 'Parent', ax, 'PreservePlot', false);
    drawnow;

    % Crear deslizadores y etiquetas
    sliders = gobjects(3, 1);
    labels = gobjects(3, 1);
    coords = {'x', 'y', 'z'};
    values = [10, 10, 10]; % Posicion inicial del efector (cm)

    for idx = 1:3
        % Crear etiquetas
        labels(idx) = uicontrol('Style', 'text', 'Parent', fig, ...
                                'Position', [50, 400 - (idx-1)*50, 100, 20], ...
                                'String', sprintf('%s: %.2f', coords{idx}, values(idx)));

        % Crear deslizadores
        sliders(idx) = uicontrol('Style', 'slider', 'Parent', fig, ...
                                 'Min', -20, 'Max', 20, ...
                                 'Value', values(idx), ...
                                 'Position', [150, 405 - (idx-1)*50, 300, 20], ...
                                 'Callback', @(src, ~) actualizarPosicion());
    end

    % Callback para actualizar el robot cuando se mueve un deslizador
    function actualizarPosicion()
        try
            for idx = 1:3
                values(idx) = sliders(idx).Value;
                set(labels(idx), 'String', sprintf('%s: %.2f', coords{idx}, values(idx)));
            end
            % Calcular cinematica inversa
            q = cinematicaInversa(values(1), values(2), values(3), l1, l2, l3, l4, l5);

            % Mostrar el robot
            show(robot, q, 'Parent', ax, 'PreservePlot', false);
            drawnow;

            % Imprimir valores de q1-q6 en la ventana de comandos
            fprintf('q1: %.2f° | q2: %.2f° | q3: %.2f° | q4: %.2f° | q5: %.2f° | q6: %.2f°\n', q(1), q(2), q(3), q(4), q(5), q(6));
        catch ME
            warning('%s: %s', ME.identifier, ME.message);
        end
    end

    % Funcion para graficar la bancada elevada (base fija)
    function graficarBancadaElevada(ax)
        % Base plana
        base_x = [-3, 3, 3, -3];
        base_y = [-3, -3, 3, 3];
        base_z = [0, 0, 0, 0];
        fill3(ax, base_x, base_y, base_z, [0.6, 0.6, 0.6], 'FaceAlpha', 0.8); % Base gris

        % Columna elevada
        [X, Y, Z] = cylinder(2, 20); % Radio de 2 cm
        Z = Z * 10; % Altura de 10 cm
        surf(ax, X, Y, Z, 'FaceColor', [0.5 0.5 0.5], 'EdgeColor', 'none');
    end

    % Funcion para crear un cuerpo con una articulacion y limites
    function body = crearCuerpo(bodyName, jointName, jointType, translation, axis, limits)
        body = rigidBody(bodyName);
        joint = rigidBodyJoint(jointName, jointType);
        setFixedTransform(joint, trvec2tform(translation));
        joint.JointAxis = axis;
        joint.PositionLimits = deg2rad(limits); % Aplicar limites de 0 a 140 grados
        body.Joint = joint;
    end

    % Funcion de cinematica inversa
    function q = cinematicaInversa(px, py, pz, l1, l2, l3, l4, l5)
        % Resolver q1
        q1 = atan2d(py, px);

        % Calcular distancias
        d_xy = sqrt(px^2 + py^2);
        d_eff = sqrt(d_xy^2 + (pz - l1)^2);
        d_eff = min(max(d_eff, l3 - l4), l3 + l4); % Restringir al alcance fisico

        % Resolver q3
        cos_q3 = (l3^2 + l4^2 - d_eff^2) / (2 * l3 * l4);
        q3 = acosd(cos_q3);

        % Resolver q2
        theta = atan2d(pz - l1, d_xy);
        phi1 = acosd((l3^2 + d_eff^2 - l4^2) / (2 * l3 * d_eff));
        q2 = theta - phi1;

        % Resolver q4, q5, q6 con limites
        q4 = -q2 - q3; % Doblar codo
        q5 = 0;        % Mantener fija la orientacion (puede modificarse)
        q6 = 0;

        % Aplicar limites a los angulos
        q = min(max([q1, q2, q3, q4, q5, q6], 0), 140);
    end
end
