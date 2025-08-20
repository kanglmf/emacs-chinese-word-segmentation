# https://github.com/hankcs/HanLP/blob/doc-zh/plugins/hanlp_demo/hanlp_demo/zh/tok_mtl.ipynb

import sys, os, argparse
import socket
import selectors

import hanlp

sel = selectors.DefaultSelector()

def accept_connection(server_socket):
    client_socket, addr = server_socket.accept()
    print(f"Connected to {addr}")
    client_socket.setblocking(False)
    sel.register(client_socket, selectors.EVENT_READ, handle_client)


def handle_client(client_socket, size=1024):
    try:
        data = client_socket.recv(size)
        if data:
            data = str(data, encoding='utf-8').strip()
            output = _hanlp(HanLP, data)
            client_socket.sendall(bytes(f'{data}: {output}\n', 'utf-8'))
            print(f'{data}: {output}', file=sys.stderr)
        else:
            sel.unregister(client_socket)
            print(f'closed: {str(client_socket)}')
            client_socket.close()
    except Exception as err:
        print(f"Error for data: {str(err)}", file=sys.stderr)
        sel.unregister(client_socket)
        print(f'closed: {str(client_socket)}', file=sys.stderr)
        client_socket.close()


def _hanlp(handle, sentence, task_type = 'tok/fine'):
    seg = handle(sentence, tasks=task_type)
    # seg_len = list(map(len, seg[task_type]))
    return ' '.join([str(i) for i in map(len, seg[task_type])])


def main(host, port, size=1024):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((host, port))
    sock.listen()
    sock.setblocking(False)
    sel.register(sock, selectors.EVENT_READ, accept_connection)

    print(f"Server listening on port {port}")
    while True:
        events = sel.select()
        for key, _ in events:
            callback = key.data
            callback(key.fileobj)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Socket server for hanlp.')
    parser.add_argument('--host', help='host address', required=True)
    parser.add_argument('--port', help='port number', required=True)
    argv = parser.parse_args()

    model = hanlp.pretrained.mtl.CLOSE_TOK_POS_NER_SRL_DEP_SDP_CON_ELECTRA_BASE_ZH
    # task_type = 'tok/fine'
    HanLP = hanlp.load(model)
    print(f'HanLP model loaded ({str(model)})', file=sys.stderr)

    main(argv.host, int(argv.port))
