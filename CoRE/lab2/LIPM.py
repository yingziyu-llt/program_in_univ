from math import sqrt, sinh, cosh
import numpy as np


class LIPM:
    def __init__(self, x0=0, v0=0, z0=1, delta_t=0.001):
        self.x0 = x0
        self.v0 = v0

        self.zc = z0  # z constant

        self.delta_t = delta_t
        self.xt = x0  # center of mass position at time t
        self.zt = z0
        self.vt = v0
        self.g = 9.8  # gravity acceleration constant
        self.Tc = sqrt(self.zc / self.g)  # constant due to our assumption
        self.t = 0

        self.left_foot_x = 0
        self.left_foot_z = 0
        self.right_foot_x = 0
        self.right_foot_z = 0
        self.support_leg = "left_leg"  # left_leg or right_leg

        self.orbital_energy = 0
        self.target_orbital_energy = 0
        self.capture_point = 0

    def update(self):
        self.t += self.delta_t
        self.xt, self.vt = self.updateXtVt(self.x0, self.v0, self.t)

    def updateXtVt(self, x0, v0, t):
        ##update xt and vt at time t
        xt = x0 * cosh(t / self.Tc) + self.Tc * v0 * sinh(t / self.Tc)
        vt = x0 / self.Tc * sinh(t / self.Tc) + v0 * cosh(t / self.Tc)
        ## End of TODO #########################################################

        return xt, vt

    def getOrbitalEnergy(self, x, v):
        ## compute the orbital energy
        ## for each single cycle, the orbital energy keeps constant
        orbital_energy = (
            1 / 2.0 * v * v - self.g / 2 / self.zc * x * x
        )  # 这里实际上用zc和zt是一样的，因为这是一个LIPM,z不变
        ## End of TODO #########################################################

        return orbital_energy

    def updateCapturePoint(self, xt, vt, target_OE):
        if vt**2 < 2 * target_OE:
            capture_point = self.capture_point
            print(
                "warning: calculate the capture point failed, the velocity is too low."
            )
        else:
            ## update the capture point associated with the target orbital energy
            capture_point = xt + (vt / abs(vt)) * sqrt(
                self.zc / self.g * (vt * vt - 2 * target_OE)
            )
            ## End of TODO #########################################################

        return capture_point

    # switch the support leg for continue walking
    def switchSupportLeg(self):
        if self.support_leg == "left_leg":
            self.x0 = self.left_foot_x + self.xt - self.right_foot_x
            self.support_leg = "right_leg"

        elif self.support_leg == "right_leg":
            self.x0 = self.right_foot_x + self.xt - self.left_foot_x
            self.support_leg = "left_leg"
        else:
            print("Error: support leg is a wrong value:", self.support_leg)

        print("t = %.2f" % self.t + ", switch the support leg to " + self.support_leg)
        self.t = 0  # reset t
        self.v0 = self.vt
