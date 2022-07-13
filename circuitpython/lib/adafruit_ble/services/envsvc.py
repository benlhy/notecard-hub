# 2022 Ben 

from . import Service
from ..uuid import VendorUUID
from ..uuid import StandardUUID
from ..characteristics.string import StringCharacteristic
from ..characteristics import Characteristic

class ENVService(Service):
    uuid=StandardUUID(0x181A)
    temp_c = Characteristic(
        uuid=StandardUUID(0x2A1F),
        #properties=Characteristic.READ,

    )
    temp = Characteristic(
        uuid=StandardUUID(0x2A6E)
    )
    hum = Characteristic(
        uuid=StandardUUID(0x2A6F)
    )

    def __init__(
        self,
        *,
        temp_c=None,
        temp=None,
        hum=None,
        service=None
        ):
        super().__init__(
            temp_c = temp_c,
            temp = temp,
            hum = hum,
            service=service

        )
        
        
    
    