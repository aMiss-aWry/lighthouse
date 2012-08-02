from SocketServer import ForkingTCPServer, StreamRequestHandler
from time import gmtime, strftime
from BTO import BTO_Server, BTORequestHandler


class LhRequestHandler(StreamRequestHandler, BTORequestHandler):
    
    def __init__(self, request, client_address, server, legal_options,
                 legal_longoption, users, req_id, bto_dir, bto_blas):
        BTORequestHandler.__init__(self, legal_options, legal_longoption, 
                                   users, req_id, bto_dir, bto_blas)
        StreamRequestHandler.__init__(self, request, client_address, server)
    
    def handle(self):
        self.bto_handle()
    
    
class LhServer(ForkingTCPServer, BTO_Server):
    
    def __init__(self, server_address, RequestHandlerClass,
                 u=[],r='1', btodir='../bto', btoblas='./bin/btoblas'):
        BTO_Server.__init__(self, u,r,btodir, btoblas)
        ForkingTCPServer.__init__(self,server_address, RequestHandlerClass)
        
    def finish_request(self, request, client_address):
        """Finish one request by instantiating RequestHandlerClass."""
        self.RequestHandlerClass(request, client_address, self,
                self.legal_options,self.legal_longoptions,
                self.users,self.req_id,self.bto_dir,self.bto_blas)
		
	
	

HOST = 'localhost'
PORT = 9999
USER = 'dljohnso'
req_id = strftime('%H-%M-%S',gmtime())

svr = LhServer((HOST, PORT), LhRequestHandler,[USER],req_id)
svr.serve_forever()