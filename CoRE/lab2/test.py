import matplotlib.pyplot as plt
import matplotlib.animation as animation
import argparse
import numpy as np

from LIPM import LIPM


def initAnimation():
    LIPM_left_leg_ani.set_data([], [])
    LIPM_right_leg_ani.set_data([], [])
    LIPM_COM_ani.set_data(COM_x[0], COM_z[0])
    COM_pos.set_text('')
    return [LIPM_left_leg_ani, LIPM_right_leg_ani, LIPM_COM_ani, COM_pos]

def animate(i):
    left_leg_x = [COM_x[i], left_foot_x_array[i]]
    left_leg_y = [COM_z[i], left_foot_z_array[i]]
    right_leg_x = [COM_x[i], right_foot_x_array[i]]
    right_leg_y = [COM_z[i], right_foot_z_array[i]]

    LIPM_left_leg_ani.set_data(left_leg_x, left_leg_y)
    LIPM_right_leg_ani.set_data(right_leg_x, right_leg_y)
    LIPM_COM_ani.set_data(COM_x[i], COM_z[i])
    COM_pos.set_text(COM_str % (COM_x[i], COM_z[i]))
    return [LIPM_left_leg_ani, LIPM_right_leg_ani, LIPM_COM_ani, COM_pos]


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Task selection script')
    parser.add_argument(
        '--task',
        type=str,
        choices=['single_step', 'multi_step'],
        required=True,
        help='Select the task type: single_step or multi_step'
    )
    args = parser.parse_args()
    
    if args.task == 'single_step':
        print("Running single-step task...")

        # Task 1: Build LIPM for single leg

        x0 = -1.0
        v0 = 3.5
        z0 = 1.0
        delta_t = 0.02
        LIPM_model = LIPM(x0, v0, z0, delta_t)

        data_len = 60
        vt_array = list()
        COM_x = list()
        COM_z = list()
        left_foot_x_array = list()
        left_foot_z_array = list()
        right_foot_x_array = list()
        right_foot_z_array = list()

        for i in range(data_len):
            LIPM_model.update()

            # save the trajectory for visualization
            vt_array.append(LIPM_model.vt)
            COM_x.append(LIPM_model.xt)
            COM_z.append(LIPM_model.zt)
            left_foot_x_array.append(LIPM_model.left_foot_x)
            left_foot_z_array.append(LIPM_model.left_foot_z)
            right_foot_x_array.append(LIPM_model.right_foot_x)
            right_foot_z_array.append(LIPM_model.right_foot_z)


        # visualization
        fig = plt.figure()
        ax = fig.add_subplot(111, autoscale_on=False, xlim=(-2, 2), ylim=(-0.2, 1.2))
        ax.grid(ls='--')

        LIPM_left_leg_ani, = ax.plot([], [], 'o-', lw=4, color='b')
        LIPM_right_leg_ani, = ax.plot([], [], 'o-', lw=4, color='k')
        LIPM_COM_ani, = ax.plot(COM_x[0], COM_z[0], marker='o', markersize=20, color='r')

        COM_str = 'COM = (%.1f, %.1f)'
        COM_pos = ax.text(0.05, 0.9, '', transform=ax.transAxes)

        ani = animation.FuncAnimation(fig=fig, init_func=initAnimation, func=animate, frames=range(1, data_len), interval=1.0/delta_t, blit=True)
        # ani.save('single_step.mp4', writer='ffmpeg', fps=30, bitrate=1800)
        plt.show()
        

    elif args.task == 'multi_step':
        print("Running multi-step task...")

        # Task 2: Build LIPM for dual legs

        x0 = -1
        v0 = 5
        z0 = 1.0
        delta_t = 0.02
        LIPM_model = LIPM(x0, v0, z0, delta_t)

        vt_array = list()
        COM_x = list()
        COM_z = list()
        left_foot_x_array = list()
        left_foot_z_array = list()
        right_foot_x_array = list()
        right_foot_z_array = list()
        
        i = 0
        data_len = 250
        while i <= data_len:
            i += 1

            # switch support leg
            if LIPM_model.xt > 1:
                target_OE = LIPM_model.getOrbitalEnergy(LIPM_model.xt, LIPM_model.vt)
                capture_point = LIPM_model.updateCapturePoint(LIPM_model.xt, LIPM_model.vt, target_OE)

                if LIPM_model.support_leg == 'right_leg':
                    LIPM_model.left_foot_x = LIPM_model.right_foot_x + capture_point
                else:
                    LIPM_model.right_foot_x = LIPM_model.left_foot_x + capture_point
                LIPM_model.switchSupportLeg()


            LIPM_model.update()

            # save the trajectory for visualization
            vt_array.append(LIPM_model.vt)
            COM_x.append(LIPM_model.xt + LIPM_model.left_foot_x if LIPM_model.support_leg == 'left_leg' else LIPM_model.xt + LIPM_model.right_foot_x)
            COM_z.append(LIPM_model.zt)
            left_foot_x_array.append(LIPM_model.left_foot_x)
            left_foot_z_array.append(LIPM_model.left_foot_z)
            right_foot_x_array.append(LIPM_model.right_foot_x)
            right_foot_z_array.append(LIPM_model.right_foot_z)


        # visualization
        fig = plt.figure()
        ax = fig.add_subplot(111, autoscale_on=False, xlim=(-2, 20), ylim=(-0.2, 1.2))
        ax.grid(ls='--')

        LIPM_left_leg_ani, = ax.plot([], [], 'o-', lw=4, color='b')
        LIPM_right_leg_ani, = ax.plot([], [], 'o-', lw=4, color='k')
        LIPM_COM_ani, = ax.plot(COM_x[0], COM_z[0], marker='o', markersize=20, color='r')

        COM_str = 'COM = (%.1f, %.1f)'
        COM_pos = ax.text(0.05, 0.9, '', transform=ax.transAxes)

        ani = animation.FuncAnimation(fig=fig, init_func=initAnimation, func=animate, frames=range(1, data_len), interval=50, blit=True)
        # ani.save('multi_step.mp4', writer='ffmpeg', fps=30, bitrate=1800)
        plt.show()


