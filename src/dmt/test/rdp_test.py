#===============================================================================
# Proof of concept for the Ramer-Douglas-Peucker algorithm for simplifying JUCE
# paths in our components.
import math
import matplotlib.pyplot as plt
#===============================================================================
SAMPLE_RATE = 44100
FREQUENCY = 60
SAMPLES_PER_CYCLE = SAMPLE_RATE / FREQUENCY
EPSILON = 0.01
#===============================================================================
def perpendicular_distance(point, start, end):
    if start == end:
        return math.sqrt((point[0] - start[0]) ** 2 + (point[1] - start[1]) ** 2)
    else:
        num = abs((end[1] - start[1]) * point[0] - (end[0] - start[0]) * point[1] + end[0] * start[1] - end[1] * start[0])
        denom = math.sqrt((end[1] - start[1]) ** 2 + (end[0] - start[0]) ** 2)
        return num / denom
def rdp(points, epsilon):
    if len(points) < 3:
        return points
    start, end = points[0], points[-1]
    dmax = 0
    index = 0
    for i in range(1, len(points) - 1):
        d = perpendicular_distance(points[i], start, end)
        if d > dmax:
            index = i
            dmax = d
    if dmax > epsilon:
        left = rdp(points[:index + 1], epsilon)
        right = rdp(points[index:], epsilon)
        return left[:-1] + right
    else:
        return [start, end]
#===============================================================================
def generate_sine_data():
    result = []
    for i in range(0, int(SAMPLES_PER_CYCLE)):
        y = math.sin(2 * math.pi * i / SAMPLES_PER_CYCLE)
        result.append(y)
    return result
def plot_data(data):
    plt.plot(data, label="Original Data")
    points = [[i, y] for i, y in enumerate(data)]
    simplified = rdp(points, EPSILON)
    print("Original Data Points: ", len(data))
    print("Simplified Data Points: ", len(simplified))
    plt.plot([p[0] for p in simplified], [p[1] for p in simplified], 'r-', label="Simplified Data")
    plt.legend()
    plt.show()

#===============================================================================
# Main
data = generate_sine_data()
plot_data(data)
