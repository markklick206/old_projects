function xa = lake_pollution_all4(~)

% This function describes the Lake Pollution Eqs
%
% dP/dt=N*P-phi*N*P
% dF/dt=lam*F-Sp*F-gam*bet*sig*N
% dN/dt=e*N*(1-(N/k))
P = 250;
F = 2000;
N = 15;

pmax = 1000;%max amount of pollution in the lake
phi = 1.001;%amount of polution humans clean up****
lam = 1;%fish reproductive rate****
gam = (pmax-P)/pmax;%porportion of pollution in the lake
Sp = 1-gam;%death rate of fish pop from pollution****
bet = 0.5;%porportion of humans that fish**
sig = 25;%number of fish each human catches**
e = 0.5;%human growth rate**
k = 1000;%human carrying capacity


% dx = zeros(3,1);
% 
% dx(1)=x(3)*x(1)-phi*x(3)*x(1);
% dx(2)=lam*x(2)-Sp*x(2)-gam*bet*sig*x(3);
% dx(3)=e*x(3)*(1-(x(3)/k));

f = @(t,x)[x(3)*x(1)-phi*x(3)*x(1);lam*x(2)-Sp*x(2)-gam*bet*sig*x(3);e*x(3)*(1-(x(3)/k));];
[t,xa] = ode45(f,[0 25],[P F N]);

figure

subplot(2,2,1)
plot(t,xa(:,1))
title('P(t)')
xlabel('t'), ylabel('Pollution')

subplot(2,2,2)
plot(t,xa(:,2))
title('F(t)')
xlabel('t'), ylabel('Fish')

subplot(2,2,3)
plot(t,xa(:,3))
title('N(t)')
xlabel('t'), ylabel('People')

subplot(2,2,4)
plot3(xa(:,1),xa(:,2),xa(:,3))
grid on
title('Solution curve')
xlabel('Pollution'), ylabel('Fish'), zlabel('People')
end

