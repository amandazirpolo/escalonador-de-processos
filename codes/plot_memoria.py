import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as patches


def plot_memory(pages, length):
    # Definindo o tamanho da memória
    memory_size = length
    # Criando a figura e o eixo
    fig, ax = plt.subplots(figsize=(10, 2))

    # Ajustando a margem da figura
    fig.subplots_adjust(left=0.05, right=0.85, top=0.85, bottom=0.2)

    # Criando um mapa de cores, onde 0 (sem processo) será branco e cada processo terá uma cor única
    unique_processes = list(set(pages))
    colors = plt.cm.tab20(np.linspace(0, 1, len(unique_processes)))

    # Definindo a cor branca para o processo 'Free'
    free_color = [1, 1, 1, 1]  # RGBA para branco
    color_mapping = {page: colors[unique_processes.index(page)] if page != 0 else free_color for page in
                     unique_processes}

    # Criando uma matriz de cores
    color_matrix = np.array([color_mapping[page] for page in pages])

    # Convertendo a matriz de cores para 2D
    color_matrix = color_matrix.reshape(1, -1, 4)

    # Desenhando a memória como um conjunto de retângulos coloridos
    ax.imshow(color_matrix, aspect='auto', interpolation='nearest')

    # Adicionando bordas pretas a cada página
    for i in range(memory_size):
        rect = patches.Rectangle((i - 0.5, -0.5), 1, 1, linewidth=1, edgecolor='black', facecolor='none')
        ax.add_patch(rect)

    # Ajustando os eixos
    ax.set_xticks(range(memory_size))
    ax.set_yticks([])
    ax.set_xticklabels(range(memory_size))
    ax.set_xlim(-0.5, memory_size - 0.5)

    # Adicionando legendas
    legend_handles = [plt.Line2D([0], [0], color=color_mapping[page], lw=4) for page in unique_processes]
    legend_labels = [f'Process {p}' if p != 0 else 'Free' for p in unique_processes]
    ax.legend(legend_handles, legend_labels, bbox_to_anchor=(1.0, 1), loc='upper left')

    # Adicionando o título
    ax.set_title("Alocação de páginas na RAM")

    # Salvar o gráfico como um arquivo
    plt.savefig('memory_plot.png')
    print("Gráfico salvo como memory_plot.png")


# Exemplo de uso para teste
if __name__ == "__main__":
    pages = [1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0]
    plot_memory(pages, len(pages))
