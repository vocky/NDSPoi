# -*- coding:utf-8 -*-

from sqlrest import create_app


if __name__ == '__main__':
    app = application = create_app()
    #app.debug = True 
    if not app.debug:
        import logging
        logging.basicConfig(level=logging.DEBUG,
                format='%(asctime)s %(levelname)s %(message)s',
                datefmt='%a, %d %b %Y %H:%M:%S',
                filename='logs/pro.log',
                filemode='w')
        logging.debug('dddddddddd')
    app.run(host="0.0.0.0", port=5005)
